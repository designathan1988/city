// `converter pack`: a port committed on a workspace's branch, as a conversion package (patch series and manifest),
// which `converter sandbox` then verifies from a fresh clone of the game. The source files it records are the ones the
// feature's extraction wrote (`converter feature`, its source/ folder).
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import { readWorkspace } from '../sandbox/workspace.ts';
import { packageBranch } from './create.ts';

export const PACK_FLAGS: readonly FlagSpec[] = [
  { name: 'port', required: true, help: 'port folder made by `converter workspace`' },
  { name: 'feature-dir', required: true, help: 'the feature\'s extraction (`converter feature --out`): its source/ lists the source files read' },
  { name: 'source-root', required: true, help: 'root of the source project (read only)' },
  { name: 'out', required: true, help: 'package folder to write (outside the game, empty)' },
];

/** The files under `dir`, relative to it, with forward slashes. */
function filesUnder(dir: string): string[] {
  return fs.readdirSync(dir, { recursive: true, withFileTypes: true }).filter((e) => e.isFile())
    .map((e) => path.relative(dir, path.join(e.parentPath, e.name)).split(path.sep).join('/')).sort();
}

export function runPackCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, PACK_FLAGS);
  const record = readWorkspace(flags.require('port'));
  const sources = path.join(path.resolve(flags.require('feature-dir')), 'source');
  if (!fs.existsSync(sources)) throw new ArgsError(`${sources} does not exist: give the folder \`converter feature\` wrote`);
  const manifest = packageBranch({
    feature: record.feature, sourceRoot: flags.require('source-root'), sourceFiles: filesUnder(sources), gameRoot: record.game,
    repo: record.workspace, base: record.base, branch: record.branch, outDir: flags.require('out'),
  });
  console.log(`package ${record.feature}: ${manifest.patches.length} patches on ${manifest.game.base.slice(0, 12)}, ` +
    `${manifest.source.files.length} source files recorded -> ${path.resolve(flags.require('out'))}`);
  for (const patch of manifest.patches) console.log(`  ${patch}`);
}
