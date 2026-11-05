// Type-checks the converter with a TypeScript package and an @types folder given on the command line, so the
// converter needs no install of its own: `node scripts/typecheck.ts --typescript <dir> --types <node_modules/@types>`.
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { createRequire } from 'node:module';
import { parseFlags } from '../src/common/args.ts';

const flags = parseFlags(process.argv.slice(2), [
  { name: 'typescript', required: true, help: 'typescript package directory' },
  { name: 'types', required: true, help: 'a node_modules/@types directory that holds @types/node' },
]);
const typescriptDir = path.resolve(flags.require('typescript'));
const ts = createRequire(path.join(typescriptDir, 'package.json'))(typescriptDir) as typeof import('typescript');
const here = path.resolve(import.meta.dirname, '..');
const config = path.join(os.tmpdir(), `converter-typecheck-${process.pid}.json`);
fs.writeFileSync(config, JSON.stringify({
  extends: path.join(here, 'tsconfig.json'),
  compilerOptions: {
    paths: { typescript: [path.join(typescriptDir, 'lib', 'typescript.d.ts')] },
    typeRoots: [path.resolve(flags.require('types'))],
  },
  include: [path.join(here, 'src'), path.join(here, 'scripts'), path.join(here, 'test', '*.ts')],
}));
try {
  const parsed = ts.getParsedCommandLineOfConfigFile(config, {}, {
    ...ts.sys,
    onUnRecoverableConfigFileDiagnostic: (d) => { throw new Error(ts.flattenDiagnosticMessageText(d.messageText, '\n')); },
  });
  if (!parsed) throw new Error('could not parse the generated tsconfig');
  const program = ts.createProgram(parsed.fileNames, parsed.options);
  const diagnostics = [...parsed.errors, ...ts.getPreEmitDiagnostics(program)];
  const host: import('typescript').FormatDiagnosticsHost = {
    getCanonicalFileName: (f) => f,
    getCurrentDirectory: () => here,
    getNewLine: () => '\n',
  };
  if (diagnostics.length > 0) console.error(ts.formatDiagnostics(diagnostics, host));
  console.log(`typecheck: ${parsed.fileNames.length} files, ${diagnostics.length} problems (TypeScript ${ts.version})`);
  process.exitCode = diagnostics.length > 0 ? 1 : 0;
} finally {
  fs.rmSync(config, { force: true });
}
