// `converter graph`: builds the graph IR of the source project and writes it as JSON (and optionally Markdown).
import fs from 'node:fs';
import path from 'node:path';
import { parseFlags, type FlagSpec } from '../common/args.ts';
import { buildGraph } from './build.ts';
import { renderMarkdown } from './markdown.ts';

export const GRAPH_FLAGS: readonly FlagSpec[] = [
  { name: 'source-root', required: true, help: 'root of the TypeScript project to analyse' },
  { name: 'out', required: true, help: 'JSON file to write the graph IR to' },
  { name: 'tsconfig', help: 'tsconfig of the source project (default: <source-root>/tsconfig.json)' },
  { name: 'typescript', help: 'typescript package directory (default: the one the source project resolves)' },
  { name: 'entry', repeatable: true, help: 'file or file:Name or file:Class.member, relative to the source root; none = whole scope' },
  { name: 'include', repeatable: true, help: 'directory holding the project\'s own code, relative to the source root (default: src)' },
  { name: 'exclude', repeatable: true, help: 'directory to leave out of the scope, relative to the source root' },
  { name: 'markdown', help: 'also write a readable report (call tree and code) to this file' },
];

export function runGraph(argv: readonly string[]): void {
  const flags = parseFlags(argv, GRAPH_FLAGS);
  const ir = buildGraph({
    sourceRoot: flags.require('source-root'),
    tsconfig: flags.one('tsconfig'),
    typescriptDir: flags.one('typescript'),
    entries: flags.all('entry'),
    include: flags.all('include'),
    exclude: flags.all('exclude'),
  });
  const out = path.resolve(flags.require('out'));
  fs.mkdirSync(path.dirname(out), { recursive: true });
  fs.writeFileSync(out, JSON.stringify(ir), 'utf8');
  const markdown = flags.one('markdown');
  if (markdown !== undefined) {
    fs.mkdirSync(path.dirname(path.resolve(markdown)), { recursive: true });
    fs.writeFileSync(markdown, renderMarkdown(ir), 'utf8');
  }
  const { stats } = ir;
  console.log(`graph: ${stats.units} units, ${stats.edges} edges, ${ir.external.length} external APIs, ` +
    `${ir.unresolvedModules.length} unresolved modules, ${stats.files} files (${stats.lines} lines), ${stats.milliseconds} ms -> ${out}`);
}
