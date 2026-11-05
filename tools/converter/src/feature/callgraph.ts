// Which functions a call runs, from Jelly's call graph (cs-au-dk/jelly: a points-to analysis of the whole program, so
// a call through an interface, a callback or a function kept in an object reaches the function the value really is).
// Jelly is run on the program's roots, or its call graph given; its locations are matched to the TypeScript program's
// nodes. Jelly parses with Babel: a call has the same range in both; a function ends where it ends in both, but its
// start differs where TypeScript counts modifiers (`export function f` starts at `export` for TypeScript, at
// `function` for Babel), so a function is matched by its end and the nearest start before Babel's.
import { spawnSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';

/** Jelly's call graph JSON (its `typings/callgraph.d.ts`): locations are `file:line:column:line:column`, 1-based. */
export interface JellyCallGraph {
  readonly files: readonly string[];
  readonly functions: Readonly<Record<string, string>>;
  readonly calls: Readonly<Record<string, string>>;
  readonly call2fun: readonly (readonly [number, number])[];
}

/** What Jelly wrote of a program: its call graph and its points-to sets (a file), both relative to `base`. */
export interface JellyAnalysis {
  readonly callGraph: JellyCallGraph;
  readonly tokens: string;
  readonly base: string;
}

export const CALL_GRAPH_FILE = 'callgraph.json';
export const TOKENS_FILE = 'tokens.json';

/**
 * Runs Jelly (the package directory given) on the program's roots, in `base` (its paths are then relative to it), into
 * `dir`. Only calls the program writes are edges: a callback the platform or a package calls is not given that call's
 * arguments, so its parameters are what comes from outside.
 */
export function runJelly(jellyDir: string, base: string, roots: readonly string[], dir: string): JellyAnalysis {
  const main = path.join(path.resolve(jellyDir), 'lib', 'main.js');
  if (!fs.existsSync(main)) throw new Error(`no Jelly at ${jellyDir} (expected lib/main.js of the @cs-au-dk/jelly package)`);
  fs.mkdirSync(dir, { recursive: true });
  const cg = path.resolve(dir, CALL_GRAPH_FILE);
  const tokens = path.resolve(dir, TOKENS_FILE);
  const run = spawnSync(process.execPath, ['--max-old-space-size=12288', main, '-b', '.', '-j', cg, '--tokens-json', tokens, '--no-tty',
    '--no-print-progress', '--no-callgraph-native', '--no-callgraph-external', ...roots.map((r) => path.relative(base, r))],
  { cwd: base, encoding: 'utf8', maxBuffer: 256 * 1024 * 1024 });
  if (run.status !== 0 || !fs.existsSync(cg) || !fs.existsSync(tokens)) throw new Error(`Jelly failed (exit ${run.status}):\n${(run.stderr || run.stdout).slice(-4000)}`);
  return readJelly(dir, base);
}

/** What Jelly wrote in `dir` (by `runJelly`, run in `base`). */
export function readJelly(dir: string, base: string): JellyAnalysis {
  return { callGraph: JSON.parse(fs.readFileSync(path.join(dir, CALL_GRAPH_FILE), 'utf8')) as JellyCallGraph, tokens: path.join(dir, TOKENS_FILE), base };
}

export class CallTargets {
  readonly #ts: typeof TS;
  readonly #byCall = new Map<string, string[]>();
  readonly #base: string;
  readonly #program: (file: string) => TS.SourceFile | undefined;
  /** Function-like nodes of a file, by end position. */
  readonly #functions = new Map<string, Map<number, TS.SignatureDeclaration[]>>();

  /** `base`: the directory Jelly's paths are relative to; `program`: a source file of the program by absolute path. */
  constructor(ts: typeof TS, cg: JellyCallGraph, base: string, program: (file: string) => TS.SourceFile | undefined) {
    this.#ts = ts;
    this.#base = path.resolve(base);
    this.#program = program;
    // A location with its file made absolute (calls are compared ignoring case, as Windows paths are).
    const absolute = (loc: string): string => {
      const [file, ...rest] = loc.split(':');
      return `${path.resolve(this.#base, cg.files[Number(file)]!)}:${rest.join(':')}`;
    };
    for (const [call, fn] of cg.call2fun) {
      const key = absolute(cg.calls[String(call)]!).toLowerCase();
      (this.#byCall.get(key) ?? this.#byCall.set(key, []).get(key)!).push(absolute(cg.functions[String(fn)]!));
    }
  }

  /** The functions of the program a call runs (none: a call of the platform, of a package, or one Jelly saw no target of). */
  of(call: TS.CallExpression | TS.NewExpression): TS.SignatureDeclaration[] {
    const sf = call.getSourceFile();
    const at = (pos: number): string => {
      const lc = sf.getLineAndCharacterOfPosition(pos);
      return `${lc.line + 1}:${lc.character + 1}`;
    };
    const key = `${path.resolve(sf.fileName).toLowerCase()}:${at(call.getStart(sf))}:${at(call.end)}`;
    return (this.#byCall.get(key) ?? []).map((loc) => this.#function(loc)).filter((f): f is TS.SignatureDeclaration => f !== null);
  }

  #function(loc: string): TS.SignatureDeclaration | null {
    const m = /^(.*):(\d+):(\d+):(\d+):(\d+)$/.exec(loc);
    if (!m) return null;
    const sf = this.#program(m[1]!);
    if (!sf) return null;
    const pos = (line: string, column: string): number => sf.getPositionOfLineAndCharacter(Number(line) - 1, Number(column) - 1);
    const start = pos(m[2]!, m[3]!);
    const end = pos(m[4]!, m[5]!);
    let byEnd = this.#functions.get(m[1]!);
    if (!byEnd) {
      const found = new Map<number, TS.SignatureDeclaration[]>();
      const visit = (n: TS.Node): void => {
        if (this.#ts.isFunctionLike(n)) (found.get(n.end) ?? found.set(n.end, []).get(n.end)!).push(n);
        this.#ts.forEachChild(n, visit);
      };
      visit(sf);
      byEnd = found;
      this.#functions.set(m[1]!, byEnd);
    }
    // Of the functions ending there, the one starting nearest before (or at) Babel's start: the innermost that holds it.
    let best: TS.SignatureDeclaration | null = null;
    for (const f of byEnd.get(end) ?? []) if (f.getStart(sf) <= start && (!best || f.getStart(sf) > best.getStart(sf))) best = f;
    return best;
  }
}
