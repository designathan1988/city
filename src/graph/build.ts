// Builds the graph IR of a TypeScript project: from named entries (closing over everything they use) or, with no
// entry, from every declaration in the scope directories.
import path from 'node:path';
import { performance } from 'node:perf_hooks';
import { isInside, normalizePath, relativePath } from '../common/paths.ts';
import { IR_VERSION, type GraphIR } from './ir.ts';
import { edgeRecords, externalSummary, unitRecord } from './records.ts';
import { Registry } from './registry.ts';
import { loadTypeScript, openProgram, type SourceProgram } from './typescript.ts';
import { nameOf, statementUnits } from './units.ts';
import { Walker } from './walk.ts';

export interface GraphOptions {
  readonly sourceRoot: string;
  /** Defaults to `<sourceRoot>/tsconfig.json`. */
  readonly tsconfig?: string | undefined;
  /** The `typescript` package directory; defaults to the one the source project resolves. */
  readonly typescriptDir?: string | undefined;
  /** `file` or `file:Name` / `file:Class.member`, relative to the source root. Empty: the whole scope. */
  readonly entries: readonly string[];
  /** Directories whose declarations are the project's own code (default `src`), relative to the source root. */
  readonly include: readonly string[];
  readonly exclude: readonly string[];
}

export function buildGraph(options: GraphOptions): GraphIR {
  const started = performance.now();
  const root = normalizePath(options.sourceRoot);
  const ts = loadTypeScript(root, options.typescriptDir);
  const sp = openProgram(ts, root, options.tsconfig ?? path.join(root, 'tsconfig.json'));
  const include = (options.include.length > 0 ? options.include : ['src']).map((d) => normalizePath(path.resolve(root, d)));
  const exclude = options.exclude.map((d) => normalizePath(path.resolve(root, d)));
  const inScope = (file: string): boolean => {
    const normal = normalizePath(file);
    return include.some((d) => isInside(d, normal)) && !exclude.some((d) => isInside(d, normal));
  };
  const registry = new Registry(ts, root);
  const walker = new Walker(sp, registry, inScope, options.entries.length === 0);
  if (options.entries.length > 0) {
    for (const entry of options.entries) addEntry(sp, registry, root, entry);
  } else {
    const files = sp.program.getSourceFiles().filter((sf) => inScope(sf.fileName));
    files.sort((a, b) => normalizePath(a.fileName).localeCompare(normalizePath(b.fileName)));
    for (const sf of files) for (const statement of sf.statements) for (const ref of statementUnits(ts, statement)) registry.add(ref, null, null);
  }
  for (let i = 0; i < registry.queue.length; i++) walker.walk(registry.queue[i]!);

  const units = [...registry.units.values()]
    .map((unit) => unitRecord(sp, registry, unit, relativePath(root, unit.file)))
    .sort((a, b) => a.id.localeCompare(b.id));
  const files = [...new Set([...registry.units.values()].map((unit) => unit.file))]
    .map((file) => ({ path: relativePath(root, file), lines: sp.sourceFile(file)?.getLineStarts().length ?? 0 }))
    .sort((a, b) => a.path.localeCompare(b.path));
  const lines = files.reduce((sum, file) => sum + file.lines, 0);
  const edges = edgeRecords(registry);
  return {
    version: IR_VERSION,
    sourceRoot: root,
    language: 'ts',
    config: sp.tsconfig,
    analyzer: `typescript ${ts.version}`,
    entries: [...options.entries],
    include: include.map((d) => relativePath(root, d)),
    exclude: exclude.map((d) => relativePath(root, d)),
    files,
    units,
    edges,
    moduleLoads: [...walker.moduleLoads].sort((a, b) => a.from.localeCompare(b.from) || a.specifier.localeCompare(b.specifier)),
    external: externalSummary(units),
    unresolvedModules: walker.unresolvedModules
      .map((u) => ({ from: registry.units.get(u.from)!.id, specifier: u.specifier }))
      .sort((a, b) => a.from.localeCompare(b.from) || a.specifier.localeCompare(b.specifier)),
    stats: { files: files.length, units: units.length, edges: edges.length, lines, milliseconds: Math.round(performance.now() - started) },
  };
}

function addEntry(sp: SourceProgram, registry: Registry, root: string, spec: string): void {
  const { ts } = sp;
  const colon = spec.lastIndexOf(':');
  const named = colon > 1; // `C:/...` has its only colon at index 1
  const file = normalizePath(path.resolve(root, named ? spec.slice(0, colon) : spec));
  const sf = sp.sourceFile(file);
  if (!sf) throw new Error(`entry file is not part of the program: ${file}`);
  if (!named) {
    for (const statement of sf.statements) for (const ref of statementUnits(ts, statement)) registry.add(ref, null, null);
    return;
  }
  const name = spec.slice(colon + 1);
  const candidates = sf.statements.flatMap((statement) => statementUnits(ts, statement));
  const matches = candidates.filter((ref) => {
    const unitName = nameOf(ts, ref);
    return unitName === name || (!name.includes('.') && unitName.split(',').includes(name));
  });
  if (matches.length === 0) throw new Error(`no declaration named ${name} in ${relativePath(root, file)}`);
  for (const ref of matches) {
    registry.add(ref, null, null);
    // A class as entry: the whole class, every member included.
    if (ref.head && ts.isClassLike(ref.node)) for (const member of ref.node.members) registry.add({ node: member, head: false }, null, null);
  }
}
