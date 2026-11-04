// `converter feature`: the whole of a feature, cut from the program at its boundary, into an output folder:
// boundary.json (own declarations, the shared ones it uses, where the program plugs it in, external APIs),
// feature.md (the same, readable), and source/ (the feature's own code: whole files when every declaration in them
// is the feature's, else just its declarations, each with its line range).
import fs from 'node:fs';
import path from 'node:path';
import { parseFlags, type FlagSpec } from '../common/args.ts';
import { removeTree } from '../common/files.ts';
import { buildGraph } from '../graph/build.ts';
import type { GraphIR } from '../graph/ir.ts';
import { featureBoundary, htmlRoots, type FeatureBoundary, type FeatureUse } from './boundary.ts';
import { writeSources, type SourceOut } from './sources.ts';

export const FEATURE_FLAGS: readonly FlagSpec[] = [
  { name: 'source-root', required: true, help: 'root of the TypeScript project' },
  { name: 'tsconfig', help: 'tsconfig of the source project (default: <source-root>/tsconfig.json)' },
  { name: 'typescript', help: 'typescript package directory (default: the one the source project resolves)' },
  { name: 'entry', required: true, repeatable: true, help: 'a file of the feature, relative to the source root' },
  { name: 'root', repeatable: true, help: 'a script the program starts from (default: the module scripts of the root\'s HTML pages)' },
  { name: 'include', repeatable: true, help: 'directory holding the project\'s own code, relative to the source root (default: src)' },
  { name: 'out', required: true, help: 'output folder' },
];

export function runFeatureCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, FEATURE_FLAGS);
  const sourceRoot = path.resolve(flags.require('source-root'));
  const out = path.resolve(flags.require('out'));
  // The whole program's graph: whether a declaration is shared depends on everything that uses it.
  const ir = buildGraph({ sourceRoot, tsconfig: flags.one('tsconfig'), typescriptDir: flags.one('typescript'), entries: [], include: flags.all('include'), exclude: [] });
  const roots = flags.all('root').length > 0 ? flags.all('root') : htmlRoots(sourceRoot);
  const boundary = featureBoundary(ir, flags.all('entry'), roots);
  removeTree(out);
  fs.mkdirSync(out, { recursive: true });
  fs.writeFileSync(path.join(out, 'boundary.json'), JSON.stringify(boundary, null, 1), 'utf8');
  const sources = writeSources(ir, boundary, sourceRoot, path.join(out, 'source'));
  fs.writeFileSync(path.join(out, 'feature.md'), renderFeature(ir, boundary, sources), 'utf8');
  const lines = sources.reduce((n, s) => n + s.lines, 0);
  console.log(`feature ${boundary.entries.join(', ')}: ${boundary.own.length} own declarations in ${sources.length} files (${lines} lines); ` +
    `uses ${boundary.uses.length} shared, ${boundary.hooks.length} hooks, ${boundary.external.length} external APIs -> ${out}`);
}

function renderFeature(ir: GraphIR, b: FeatureBoundary, sources: SourceOut[]): string {
  const units = new Map(ir.units.map((u) => [u.id, u]));
  const where = (id: string): string => {
    const u = units.get(id)!;
    return `\`${id}\` (${u.kind}, line ${u.start.line})`;
  };
  const list = (uses: readonly FeatureUse[], by: 'unit' | 'by'): string[] => {
    const byFile = new Map<string, FeatureUse[]>();
    for (const x of uses) {
      const file = units.get(x.unit)!.file;
      (byFile.get(file) ?? byFile.set(file, []).get(file)!).push(x);
    }
    return [...byFile].sort(([a], [c]) => a.localeCompare(c)).flatMap(([file, xs]) => [`### ${file}`, '',
      ...xs.map((x) => by === 'unit' ? `- ${where(x.unit)} — ${x.kind} by ${x.by.length}: ${x.by.slice(0, 4).map((i) => `\`${i}\``).join(', ')}${x.by.length > 4 ? ', …' : ''}`
        : `- ${where(x.unit)} ← ${x.kind} from ${x.by.slice(0, 4).map((i) => `\`${i}\``).join(', ')}${x.by.length > 4 ? ', …' : ''}`), '']);
  };
  return [
    `# Feature: ${b.entries.join(', ')}`,
    '',
    `The program starts from ${b.roots.map((r) => `\`${r}\``).join(', ')}. A declaration is the feature's own when the program reaches it only through the feature.`,
    '',
    `- Own: ${b.own.length} declarations, ${sources.reduce((n, s) => n + s.lines, 0)} lines in ${sources.length} files (source/)`,
    `- Uses: ${b.uses.length} shared declarations`,
    `- Hooks: ${b.hooks.length} places where the program reaches into the feature`,
    `- External APIs: ${b.external.length}`,
    '',
    '## Own code',
    '',
    ...sources.map((s) => `- ${s.file} — ${s.whole ? 'whole file' : 'its own declarations'}, ${s.lines} lines`),
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
