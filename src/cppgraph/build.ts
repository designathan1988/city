// Builds the graph IR of a C++ project through clangd: the units of every file in scope, and from the entries (or every
// unit) each followed name resolved to its definition, as a typed edge to a unit or as a keyed external use. The IR is
// the same as the TypeScript side's, so the audit and the reports read both.
import fs from 'node:fs';
import path from 'node:path';
import { performance } from 'node:perf_hooks';
import { isInside, normalizePath, relativePath } from '../common/paths.ts';
import { externalSummary } from '../graph/records.ts';
import { IR_VERSION, type EdgeKind, type EdgeRecord, type GraphIR, type PatternSite, type UnitRecord } from '../graph/ir.ts';
import { edgeKinds, followed } from './edges.ts';
import { ExternalNames, moduleOfCpp } from './external.ts';
import { cppFacts } from './facts.ts';
import { tokenize, type CppToken } from './lexer.ts';
import { ClangdSession, type SemanticToken } from './session.ts';
import { FileText, unitsOf, type CppUnit, type LspPosition } from './symbols.ts';

export interface CppGraphOptions {
  readonly root: string;
  readonly compileCommands: string;
  readonly clangd: string;
  /** The converter's folder for the compile commands copy and clangd's index (outside the project). */
  readonly cacheDir: string;
  readonly include: readonly string[];
  readonly exclude: readonly string[];
  /** `file:Name` or `file:Class::method`, relative to the root; none: every unit in scope. */
  readonly entries: readonly string[];
  readonly clangdArgs: readonly string[];
  readonly indexTimeoutSeconds: number;
}

const SOURCE = /\.(h|hh|hpp|hxx|inl|c|cc|cpp|cxx)$/;
const HEADER = /\.(h|hh|hpp|hxx|inl)$/;
/** Definition requests in flight at once. */
const BATCH = 64;
const RS_GETTER = /^godot-cpp:RenderingServer\.(?!get_singleton$)(\w*_get\w*|get_\w+)$/;

interface Unit {
  readonly key: string;
  readonly id: string;
  readonly file: string;
  readonly u: CppUnit;
}

interface FileData {
  readonly text: FileText;
  readonly tokens: CppToken[];
  readonly at: Map<string, number>;
  readonly units: Unit[];
}

const before = (a: LspPosition, b: LspPosition): boolean => a.line < b.line || (a.line === b.line && a.character <= b.character);
const span = (u: CppUnit): number => (u.range.end.line - u.range.start.line) * 100_000 + (u.range.end.character - u.range.start.character);

function listSources(dir: string): string[] {
  if (!fs.existsSync(dir)) return [];
  return fs.readdirSync(dir, { recursive: true, withFileTypes: true })
    .filter((e) => e.isFile() && SOURCE.test(e.name))
    .map((e) => normalizePath(path.join(e.parentPath, e.name)))
    .sort();
}

export async function buildCppGraph(options: CppGraphOptions): Promise<GraphIR> {
  const started = performance.now();
  const root = normalizePath(options.root);
  const cacheDir = normalizePath(options.cacheDir);
  if (isInside(root, cacheDir)) throw new Error(`the clangd cache must be outside the project (${cacheDir})`);
  const include = (options.include.length > 0 ? options.include : ['native/src']).map((d) => normalizePath(path.resolve(root, d)));
  const exclude = options.exclude.map((d) => normalizePath(path.resolve(root, d)));
  const inScope = (file: string): boolean => include.some((d) => isInside(d, file)) && !exclude.some((d) => isInside(d, file));
  const session = await ClangdSession.start(options.clangd, options.compileCommands, cacheDir, inScope, root, options.clangdArgs, options.indexTimeoutSeconds);
  try {
    const files = new Map<string, FileData>();
    const ids = new Set<string>();
    for (const file of include.flatMap(listSources).filter(inScope)) {
      const text = new FileText(fs.readFileSync(file, 'utf8'));
      const tokens = tokenize(text.text);
      const at = new Map(tokens.map((t, i) => [`${t.line}:${t.column}`, i]));
      const units = unitsOf(text, await session.symbols(file, text.text)).map((u) => {
        let id = `${relativePath(root, file)}#${u.name}`;
        if (ids.has(id)) id = `${id}@${u.range.start.line + 1}`;
        ids.add(id);
        return { key: `${file}@${u.range.start.line}:${u.range.start.character}`, id, file, u };
      });
      files.set(file, { text, tokens, at, units });
    }
    const byKey = new Map([...files.values()].flatMap((f) => f.units.map((u) => [u.key, u] as const)));
    const locate = (file: string, p: LspPosition): Unit | null => {
      let best: Unit | null = null;
      for (const unit of files.get(file)?.units ?? []) {
        if (!before(unit.u.range.start, p) || !before(p, unit.u.range.end)) continue;
        if (!best || span(unit.u) < span(best.u)) best = unit;
      }
      return best;
    };
    const queue: string[] = options.entries.length > 0 ? entryKeys(options.entries, root, files) : [...byKey.keys()];
    const reached = new Set(queue);
    const edges = new Map<string, Map<EdgeKind, number>>();
    const external = new Map<string, Map<string, Map<EdgeKind, number>>>();
    const names = new ExternalNames();
    const overrides = new Map<string, Unit[]>();
    const tokensOf = new Map<string, SemanticToken[]>();
    const add = (from: Unit, to: Unit, kind: EdgeKind): void => {
      if (from.key === to.key) return;
      const k = `${from.key}\n${to.key}`;
      const kinds = edges.get(k) ?? new Map<EdgeKind, number>();
      kinds.set(kind, (kinds.get(kind) ?? 0) + 1);
      edges.set(k, kinds);
      if (!reached.has(to.key)) { reached.add(to.key); queue.push(to.key); }
    };
    for (let q = 0; q < queue.length; q++) {
      const unit = byKey.get(queue[q]!)!;
      const data = files.get(unit.file)!;
      let semantic = tokensOf.get(unit.file);
      if (!semantic) tokensOf.set(unit.file, (semantic = await session.tokens(unit.file, data.text.text)));
      // A class head owns its name and heritage only: its members are units of their own.
      const end = unit.u.kind === 'class' ? headEnd(unit.u, data) : unit.u.range.end;
      const own = semantic.filter((t) => followed(t) && before(unit.u.range.start, { line: t.line, character: t.column }) &&
        before({ line: t.line, character: t.column }, end));
      for (let b = 0; b < own.length; b += BATCH) {
        const batch = own.slice(b, b + BATCH);
        const answers = await Promise.all(batch.map((t) => session.definition(unit.file, { line: t.line, character: t.column })));
        for (let i = 0; i < batch.length; i++) {
          const token = batch[i]!;
          const index = data.at.get(`${token.line}:${token.column}`);
          if (index === undefined) continue;
          const kinds = edgeKinds(token, data.tokens, index);
          for (const loc of answers[i]!) {
            if (!inScope(loc.file)) {
              const name = data.tokens[index]!.text;
              const container = names.container(loc.file, loc.range.start);
              const key = `${moduleOfCpp(loc.file, root)}:${container ? container + '.' : ''}${name}`;
              const uses = external.get(unit.key) ?? new Map<string, Map<EdgeKind, number>>();
              const byKind = uses.get(key) ?? new Map<EdgeKind, number>();
              for (const kind of kinds) byKind.set(kind, (byKind.get(kind) ?? 0) + 1);
              uses.set(key, byKind);
              external.set(unit.key, uses);
              continue;
            }
            const target = locate(loc.file, loc.range.start);
            if (!target) continue;
            for (const kind of kinds) add(unit, target, kind);
            if (token.type === 'method' && token.modifiers.includes('virtual')) {
              const k = `${loc.file}@${loc.range.start.line}:${loc.range.start.character}`;
              let list = overrides.get(k);
              if (!list) {
                list = (await session.implementations(loc.file, loc.range.start)).filter((l) => inScope(l.file))
                  .map((l) => locate(l.file, l.range.start)).filter((u): u is Unit => u !== null);
                overrides.set(k, list);
              }
              for (const impl of list) add(unit, impl, 'implementation');
            }
          }
        }
      }
    }
    return assemble(root, options, files, byKey, reached, edges, external, session.version, started);
  } finally {
    await session.shutdown();
  }
}

/** Where a class head ends: its opening brace (members follow). */
function headEnd(u: CppUnit, data: FileData): LspPosition {
  const brace = data.text.slice(u.range).indexOf('{');
  if (brace < 0) return u.range.end;
  const offset = data.text.offset(u.range.start) + brace;
  const head = data.text.text.slice(0, offset);
  const line = head.split('\n').length - 1;
  return { line, character: offset - (head.lastIndexOf('\n') + 1) };
}

function entryKeys(entries: readonly string[], root: string, files: Map<string, FileData>): string[] {
  return entries.map((spec) => {
    const match = /^(.*?\.(?:h|hh|hpp|hxx|inl|c|cc|cpp|cxx)):(.+)$/.exec(spec);
    if (!match) throw new Error(`entry ${spec}: expected <source file>:<Name or Class::method>`);
    const file = normalizePath(path.resolve(root, match[1]!));
    const name = match[2]!;
    const unit = files.get(file)?.units.find((u) => u.u.name === name || u.u.name.endsWith(`::${name}`));
    if (!unit) throw new Error(`no unit named ${name} in ${relativePath(root, file)}`);
    return unit.key;
  });
}

function assemble(
  root: string, options: CppGraphOptions, files: Map<string, FileData>, byKey: Map<string, Unit>, reached: Set<string>,
  edges: Map<string, Map<EdgeKind, number>>, external: Map<string, Map<string, Map<EdgeKind, number>>>, version: string, started: number,
): GraphIR {
  const classId = new Map<string, string>();
  for (const unit of byKey.values()) if (unit.u.kind === 'class' && !classId.has(unit.u.name)) classId.set(unit.u.name, unit.id);
  const units: UnitRecord[] = [...reached].map((key) => {
    const unit = byKey.get(key)!;
    const data = files.get(unit.file)!;
    const { start, end } = unit.u.range;
    const own = data.tokens.filter((t) => before(start, { line: t.line, character: t.column }) && before({ line: t.line, character: t.column }, unit.u.kind === 'class' ? headEnd(unit.u, data) : end));
    const facts = cppFacts(own);
    const uses = [...(external.get(key) ?? [])].flatMap(([k, kinds]) => [...kinds].map(([kind, count]) => ({ key: k, kind, count })))
      .sort((a, b) => a.key.localeCompare(b.key) || a.kind.localeCompare(b.kind));
    const getters: PatternSite[] = uses.filter((u) => RS_GETTER.test(u.key))
      .map((u) => ({ id: 'sync-gpu-readback', line: start.line + 1, detail: `${u.key.slice(u.key.indexOf('.') + 1)}()` }));
    const beforeName = data.text.slice({ start, end: unit.u.selection.start });
    return {
      id: unit.id, kind: unit.u.kind, name: unit.u.name, file: relativePath(root, unit.file),
      start: { line: start.line + 1, column: start.character + 1 }, end: { line: end.line + 1, column: end.character + 1 },
      lines: end.line - start.line + 1, exported: HEADER.test(unit.file), signature: unit.u.detail,
      parent: unit.u.owner ? classId.get(unit.u.owner) ?? null : null, nested: [],
      facts: {
        external: uses, nondeterminism: [...new Set(facts.nondeterminism)].sort(), loopAllocations: facts.loopAllocations,
        numericLiterals: facts.numericLiterals, stringLiterals: facts.stringLiterals, decisions: facts.decisions, loops: facts.loops,
        awaits: 0, bitOperations: facts.bitOperations, isStatic: /\bstatic\b/.test(beforeName),
        callable: ['function', 'method', 'constructor'].includes(unit.u.kind), mutableModuleState: unit.u.mutable,
        patterns: [...facts.patterns, ...getters], fingerprints: facts.fingerprints,
      },
    };
  }).sort((a, b) => a.id.localeCompare(b.id));
  const edgeList: EdgeRecord[] = [...edges].flatMap(([k, kinds]) => {
    const [from, to] = k.split('\n') as [string, string];
    return [...kinds].map(([kind, count]) => ({ from: byKey.get(from)!.id, to: byKey.get(to)!.id, kind, count }));
  }).sort((a, b) => a.from.localeCompare(b.from) || a.to.localeCompare(b.to) || a.kind.localeCompare(b.kind));
  const fileList = [...new Set(units.map((u) => u.file))].sort()
    .map((rel) => ({ path: rel, lines: files.get(normalizePath(path.join(root, rel)))!.text.lines }));
  return {
    version: IR_VERSION, sourceRoot: root, language: 'cpp', config: normalizePath(options.compileCommands), analyzer: version,
    entries: [...options.entries], include: (options.include.length > 0 ? options.include : ['native/src']).map((d) => d.replace(/\\/g, '/')),
    exclude: [...options.exclude], files: fileList, units, edges: edgeList, moduleLoads: [], external: externalSummary(units),
    unresolvedModules: [],
    stats: { files: fileList.length, units: units.length, edges: edgeList.length, lines: fileList.reduce((n, f) => n + f.lines, 0),
      milliseconds: Math.round(performance.now() - started) },
  };
}
