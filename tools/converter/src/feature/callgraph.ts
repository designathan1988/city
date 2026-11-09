// Which functions a call runs, from Jelly's call graph (cs-au-dk/jelly: a points-to analysis of the whole program, so
// a call through an interface, a callback or a function kept in an object reaches the function the value really is).
// Jelly is run on the program's roots (a task keyed by the code it reads); its locations are matched to the TypeScript program's
// nodes. Jelly parses with Babel: a call has the same range in both; a function ends where it ends in both, but its
// start differs where TypeScript counts modifiers (`export function f` starts at `export` for TypeScript, at
// `function` for Babel), so a function is matched by its end and the nearest start before Babel's.
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';
import { keyedDir, keyOf, logOf, mustRun, passed, runTasksSync, toolKey, treeKey } from '../common/tasks.ts';

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

/** Jelly on a whole program (measured: 59 s on the 4.0's 863 files): past this its process tree is stopped. */
const JELLY_TIMEOUT_S = 1800;
/** Jelly's heap: the whole 4.0 needs more than Node's default. */
const JELLY_HEAP_MB = 12288;

/**
 * Jelly (the package directory given) on the program's roots, run in `base` (its paths are then relative to it). The
 * analysis is a task keyed by what it reads (the code under `base`, the roots, Jelly itself): it lives in the folder
 * that key names and is made only when no run made it. Only calls the program writes are edges: a callback the platform
 * or a package calls is not given that call's arguments, so its parameters are what comes from outside.
 */
export function runJelly(jellyDir: string, base: string, roots: readonly string[]): JellyAnalysis {
  const main = path.join(path.resolve(jellyDir), 'lib', 'main.js');
  if (!fs.existsSync(main)) throw new Error(`no Jelly at ${jellyDir} (expected lib/main.js of the @cs-au-dk/jelly package)`);
  const relativeRoots = roots.map((r) => path.relative(base, r).split(path.sep).join('/')).sort();
  const flags = ['--no-tty', '--no-print-progress', '--no-callgraph-native', '--no-callgraph-external'];
  const key = keyOf('jelly', treeKey(base), toolKey(jellyDir), process.version, ...flags, ...relativeRoots);
  const dir = keyedDir('jelly', key);
  fs.mkdirSync(dir, { recursive: true });
  const cg = path.join(dir, CALL_GRAPH_FILE);
  const tokens = path.join(dir, TOKENS_FILE);
  const [run] = runTasksSync([{ name: 'converter-jelly', argv: [process.execPath, `--max-old-space-size=${JELLY_HEAP_MB}`, main, '-b', '.', '-j', cg, '--tokens-json', tokens,
    ...flags, ...relativeRoots], cwd: base, extra: [key], outputs: [cg, tokens], timeoutS: JELLY_TIMEOUT_S }]);
  if (!passed(mustRun('Jelly', run!)) || !fs.existsSync(cg) || !fs.existsSync(tokens)) throw new Error(`Jelly failed (${run!.status}, exit ${run!.exit}):\n${logOf(run!).slice(-4000)}`);
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
