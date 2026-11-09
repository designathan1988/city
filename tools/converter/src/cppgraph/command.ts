// `converter cppgraph`: the graph IR of a C++ project (the target game's native code) through clangd, read only.
import crypto from 'node:crypto';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import { normalizePath } from '../common/paths.ts';
import { renderMarkdown } from '../graph/markdown.ts';
import { buildCppGraph } from './build.ts';

const DEFAULT_INDEX_TIMEOUT_S = 1200;

export const CPPGRAPH_FLAGS: readonly FlagSpec[] = [
  { name: 'root', required: true, help: 'root of the C++ project (read only)' },
  { name: 'compile-commands', required: true, help: 'its compile_commands.json (read only; a copy is indexed)' },
  { name: 'clangd', required: true, help: 'clangd executable' },
  { name: 'out', required: true, help: 'JSON file to write the graph IR to' },
  { name: 'markdown', help: 'also write a readable report (call tree and code) to this file' },
  { name: 'cache', help: 'folder for the compile commands copy and clangd\'s index (default: one per project in the temp folder)' },
  { name: 'include', repeatable: true, help: 'directory of the project\'s own code, relative to the root (default: native/src)' },
  { name: 'exclude', repeatable: true, help: 'directory to leave out, relative to the root' },
  { name: 'entry', repeatable: true, help: 'file:Name or file:Class::method, relative to the root; none = whole scope' },
  { name: 'clangd-arg', repeatable: true, help: 'extra clangd flag, e.g. --clangd-arg=--query-driver=<compiler glob>' },
  { name: 'index-timeout', help: `seconds to wait for clangd's background index (default ${DEFAULT_INDEX_TIMEOUT_S})` },
  { name: 'scope', help: '"graph" (default: units and edges, over a background index) or "declarations" (units alone, no index: what a translation reads)' },
  { name: 'symbol-cache', help: 'folder that keeps each file\'s symbols by an exact key across graphs (outside the project): only changed files are parsed' },
];

export async function runCppGraph(argv: readonly string[]): Promise<void> {
  const flags = parseFlags(argv, CPPGRAPH_FLAGS);
  const root = normalizePath(flags.require('root'));
  const timeout = Number(flags.one('index-timeout') ?? DEFAULT_INDEX_TIMEOUT_S);
  if (!Number.isFinite(timeout) || timeout <= 0) throw new ArgsError('--index-timeout must be a positive number');
  const cache = flags.one('cache') ??
    path.join(os.tmpdir(), `converter-clangd-${crypto.createHash('sha256').update(root).digest('hex').slice(0, 16)}`);
  const scope = flags.one('scope') ?? 'graph';
  if (scope !== 'graph' && scope !== 'declarations') throw new ArgsError('--scope must be "graph" or "declarations"');
  const ir = await buildCppGraph({
    root, compileCommands: flags.require('compile-commands'), clangd: flags.require('clangd'), cacheDir: cache,
    include: flags.all('include'), exclude: flags.all('exclude'), entries: flags.all('entry'), clangdArgs: flags.all('clangd-arg'),
    indexTimeoutSeconds: timeout, declarationsOnly: scope === 'declarations', symbolCacheDir: flags.one('symbol-cache'),
  });
  const out = path.resolve(flags.require('out'));
  fs.mkdirSync(path.dirname(out), { recursive: true });
  fs.writeFileSync(out, JSON.stringify(ir), 'utf8');
  const markdown = flags.one('markdown');
  if (markdown !== undefined) {
    fs.mkdirSync(path.dirname(path.resolve(markdown)), { recursive: true });
    fs.writeFileSync(markdown, renderMarkdown(ir), 'utf8');
  }
  console.log(`cppgraph: ${ir.stats.units} units, ${ir.stats.edges} edges, ${ir.external.length} external APIs, ` +
    `${ir.stats.files} files (${ir.stats.lines} lines), ${ir.stats.milliseconds} ms, ${ir.analyzer} -> ${out}`);
}
