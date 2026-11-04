// `converter translate`: a feature of the source written as C++ in its owner folder, into an output folder (never the
// target). Refused (exit 1, no files) when any construct could not be translated: the report lists each one.
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import { removeTree } from '../common/files.ts';
import { normalizePath } from '../common/paths.ts';
import { renderTranslation, translate } from './run.ts';
import { readTranslationSpec } from './spec.ts';

export const TRANSLATE_FLAGS: readonly FlagSpec[] = [
  { name: 'source-root', required: true, help: 'root of the TypeScript source project' },
  { name: 'tsconfig', help: 'the source\'s tsconfig.json (default: <source-root>/tsconfig.json)' },
  { name: 'typescript', help: 'TypeScript package directory (default: the source project\'s own)' },
  { name: 'spec', required: true, help: 'translation spec JSON (configs/<feature>.translate.json)' },
  { name: 'target-graph', required: true, help: 'the target\'s C++ graph JSON (converter cppgraph): the types it already has' },
  { name: 'target-root', required: true, help: 'root of the target game (read only: its gate rules)' },
  { name: 'out', required: true, help: 'output folder: files/<target paths>, translation.json, translation.md' },
];

export function runTranslateCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, TRANSLATE_FLAGS);
  const sourceRoot = path.resolve(flags.require('source-root'));
  const targetRoot = path.resolve(flags.require('target-root'));
  const out = path.resolve(flags.require('out'));
  if (normalizePath(out).startsWith(normalizePath(targetRoot) + '/')) throw new ArgsError(`--out must be outside the target (${out})`);
  const spec = readTranslationSpec(flags.require('spec'));
  const result = translate({ sourceRoot, tsconfig: flags.one('tsconfig') ?? path.join(sourceRoot, 'tsconfig.json'),
    typescriptDir: flags.one('typescript'), spec, targetRoot, targetGraph: path.resolve(flags.require('target-graph')) });
  // Retried removal: a file a process still holds open on Windows does not fail the run.
  removeTree(path.join(out, 'files'));
  fs.mkdirSync(out, { recursive: true });
  if (result.errors.length === 0) {
    for (const [file, text] of result.files) {
      const target = path.join(out, 'files', file);
      fs.mkdirSync(path.dirname(target), { recursive: true });
      fs.writeFileSync(target, text, 'utf8');
    }
  }
  fs.writeFileSync(path.join(out, 'translation.json'), JSON.stringify({ ...result, files: [...result.files.keys()] }, null, 1), 'utf8');
  fs.writeFileSync(path.join(out, 'translation.md'), renderTranslation(result), 'utf8');
  console.log(`translate ${result.feature}: ${result.units.length} units -> ${result.files.size} files in ${result.subsystem}; ` +
    `${result.errors.length === 0 ? 'complete' : `refused, ${result.errors.length} error(s)`}; ${result.pending.length} left to other owners -> ${out}`);
  for (const e of result.errors.slice(0, 40)) console.log(`  ${e}`);
  process.exitCode = result.errors.length === 0 ? 0 : 1;
}
