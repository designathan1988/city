// `converter feature`: the whole of a feature, cut from the program, into an output folder: boundary.json (its own
// declarations, its slices of shared code, the shared declarations it uses, where the program plugs it in, the
// external APIs and the resources it uses), feature.md (the same, readable) and source/ (its code, see sources.ts).
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import { removeTree } from '../common/files.ts';
import { buildGraph } from '../graph/build.ts';
import { IR_VERSION, type GraphIR } from '../graph/ir.ts';
import { loadTypeScript, openProgram } from '../graph/typescript.ts';
import { featureBoundary, type FeatureBoundary, type FeatureUse } from './boundary.ts';
import { holdsFeature } from './holders.ts';
import { htmlRoots } from './html.ts';
import { CallTargets, readJelly, runJelly } from './callgraph.ts';
import { PointsTo } from './pointsto.ts';
import { featureResources, type FeatureResource } from './resources.ts';
import { sliceShared, type FeatureSlice } from './slice.ts';
import { writeSources, type SourceOut } from './sources.ts';
import { UnitIndex } from './units.ts';

export const FEATURE_FLAGS: readonly FlagSpec[] = [
  { name: 'source-root', required: true, help: 'root of the TypeScript project' },
  { name: 'tsconfig', help: 'tsconfig of the source project (default: <source-root>/tsconfig.json)' },
  { name: 'typescript', help: 'typescript package directory (default: the one the source project resolves)' },
  { name: 'postcss', help: 'postcss package directory, to read style sheets (default: the one the source project resolves)' },
  { name: 'entry', required: true, repeatable: true, help: 'a file of the feature, relative to the source root' },
  { name: 'root', repeatable: true, help: 'a script the program starts from (default: the module scripts of the root\'s HTML pages)' },
  { name: 'include', repeatable: true, help: 'directory holding the project\'s own code, relative to the source root (default: src)' },
  { name: 'graph', help: 'the whole program\'s graph (`converter graph` with no entry), to reuse instead of building it' },
  { name: 'jelly', help: 'the @cs-au-dk/jelly package directory, whose call graph says what each call runs (default: $CONVERTER_JELLY)' },
  { name: 'analysis', help: 'a directory where Jelly left its call graph and points-to sets of the program (run in the code directory), to reuse' },
  { name: 'out', required: true, help: 'output folder' },
];

export function runFeatureCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, FEATURE_FLAGS);
  const sourceRoot = path.resolve(flags.require('source-root'));
  const tsconfig = flags.one('tsconfig') ?? path.join(sourceRoot, 'tsconfig.json');
  const out = path.resolve(flags.require('out'));
  const given = flags.one('graph');
  // The whole program's graph: whether a declaration is shared depends on everything that uses it.
  const ir: GraphIR = given ? JSON.parse(fs.readFileSync(given, 'utf8')) as GraphIR
    : buildGraph({ sourceRoot, tsconfig, typescriptDir: flags.one('typescript'), entries: [], include: flags.all('include'), exclude: [] });
  if (ir.version !== IR_VERSION) throw new ArgsError(`--graph ${given} is a graph of version ${ir.version}; this converter reads version ${IR_VERSION}: build it again`);
  if (ir.entries.length > 0) throw new ArgsError(`--graph ${given} is the graph of ${ir.entries.join(', ')}, not of the whole program`);
  const ts = loadTypeScript(sourceRoot, flags.one('typescript'));
  const sp = openProgram(ts, sourceRoot, tsconfig);
  const index = new UnitIndex(sp, ir);
  const roots = flags.all('root').length > 0 ? flags.all('root') : htmlRoots(sourceRoot);
  const boundary = featureBoundary(ir, flags.all('entry'), roots, holdsFeature(ts, sp.checker, index, ir));
  const slices = sliceShared(ts, sp.checker, index, boundary);
  removeTree(out);
  fs.mkdirSync(out, { recursive: true });
  // What each call runs: Jelly's call graph of the program's roots (its paths are relative to the project's own code).
  const base = path.join(sourceRoot, flags.all('include')[0] ?? 'src');
  const reuse = flags.one('analysis');
  const jelly = flags.one('jelly') ?? process.env['CONVERTER_JELLY'];
  if (!reuse && !jelly) throw new ArgsError('give --jelly (or set CONVERTER_JELLY) or --analysis: Jelly says what each call runs and what each value is');
  const analysis = reuse ? readJelly(reuse, base) : runJelly(jelly!, base, roots.map((r) => path.join(sourceRoot, r)), path.join(out, 'jelly'));
  const calls = new CallTargets(ts, analysis.callGraph, base, (file) => sp.sourceFile(file));
  const pts = new PointsTo(ts, analysis.tokens, base);
  const resources = featureResources(ts, sp.checker, index, sourceRoot, boundary, slices, calls, pts, flags.one('postcss'));
  const sources = writeSources(ir, boundary, slices, resources, sourceRoot, path.join(out, 'source'));
  const { program: _program, ...written } = boundary;
  fs.writeFileSync(path.join(out, 'boundary.json'), JSON.stringify({ ...written, slices, resources }, null, 1), 'utf8');
  fs.writeFileSync(path.join(out, 'feature.md'), renderFeature(index, boundary, slices, resources, sources), 'utf8');
  const lines = sources.reduce((n, s) => n + s.lines, 0);
  console.log(`feature ${boundary.entries.join(', ')}: ${boundary.own.length} own declarations, ${slices.length} slices of shared code, ` +
    `${resources.length} resources; ${lines} lines in ${sources.length} files; uses ${boundary.uses.length} shared, ${boundary.hooks.length} hooks, ` +
    `${boundary.external.length} external APIs -> ${out}`);
}

function renderFeature(index: UnitIndex, b: FeatureBoundary, slices: readonly FeatureSlice[], resources: readonly FeatureResource[], sources: readonly SourceOut[]): string {
  const where = (id: string): string => {
    const u = index.units.get(id)!;
    return `\`${id}\` (${u.kind}, line ${u.start.line})`;
  };
  const names = (ids: readonly string[]): string => `${ids.slice(0, 4).map((i) => `\`${i}\``).join(', ')}${ids.length > 4 ? ', …' : ''}`;
  const list = (uses: readonly FeatureUse[], by: 'unit' | 'by'): string[] => {
    const byFile = new Map<string, FeatureUse[]>();
    for (const x of uses) {
      const file = index.units.get(x.unit)!.file;
      (byFile.get(file) ?? byFile.set(file, []).get(file)!).push(x);
    }
    return [...byFile].sort(([a], [c]) => a.localeCompare(c)).flatMap(([file, xs]) => [`### ${file}`, '',
      ...xs.map((x) => by === 'unit' ? `- ${where(x.unit)} — ${x.kind} by ${x.by.length}: ${names(x.by)}` : `- ${where(x.unit)} ← ${x.kind} from ${names(x.by)}`), '']);
  };
  return [
    `# Feature: ${b.entries.join(', ')}`,
    '',
    `The program starts from ${b.roots.map((r) => `\`${r}\``).join(', ')}. A declaration is the feature's own when the program reaches it only through the feature; ` +
      'in a declaration of the program that names the feature, its statements are a slice (what names it, what depends on that, what that controls).',
    '',
    `- Own: ${b.own.length} declarations`,
    `- Slices of shared code: ${slices.length}`,
    `- Code: ${sources.reduce((n, s) => n + s.lines, 0)} lines in ${sources.length} files (source/)`,
    `- Resources: ${resources.length} (${['entry', 'style', 'import'].map((k) => `${resources.filter((r) => r.kind === k).length} ${k}`).join(', ')}; ` +
      `${resources.filter((r) => r.owner === 'feature').length} only the feature uses)`,
    `- Uses: ${b.uses.length} shared declarations; hooks: ${b.hooks.length}; external APIs: ${b.external.length}`,
    '',
    '## Code',
    '',
    ...sources.map((s) => `- ${s.file} — ${s.how}, ${s.lines} lines`),
    '',
    '## Slices of shared code',
    '',
    ...slices.map((s) => `- \`${s.unit}\` — ${s.lines.length} of ${s.end - s.start + 1} lines`),
    '',
    '## Resources',
    '',
    ...resources.map((r) => `- ${r.owner} ${r.kind} \`${r.key}\` — ${r.file}:${r.line}: \`${r.text.replace(/\s+/g, ' ').slice(0, 160)}\``),
    '',
    '## Where the program plugs the feature in',
    '',
    ...list(b.hooks, 'by'),
    '## Shared declarations the feature uses',
    '',
    ...list(b.uses, 'unit'),
    '## External APIs',
    '',
    ...b.external.map((x) => `- \`${x.key}\` × ${x.count}`),
    '',
  ].join('\n');
}
