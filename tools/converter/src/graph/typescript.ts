// Loads the TypeScript compiler the source project itself depends on (same version, same semantics) and opens the
// project's program through a language service, which also answers implementation and reference queries.
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';
import type * as TS from 'typescript';
import { normalizePath } from '../common/paths.ts';

export type TypeScriptApi = typeof TS;

export interface SourceProgram {
  readonly ts: TypeScriptApi;
  readonly root: string;
  readonly tsconfig: string;
  readonly service: TS.LanguageService;
  readonly program: TS.Program;
  readonly checker: TS.TypeChecker;
  sourceFile(file: string): TS.SourceFile | undefined;
}

/** `typescriptDir` overrides the package; by default it is resolved the way the source project resolves it. */
export function loadTypeScript(sourceRoot: string, typescriptDir: string | undefined): TypeScriptApi {
  if (typescriptDir !== undefined) {
    const dir = path.resolve(typescriptDir);
    return createRequire(path.join(dir, 'package.json'))(dir) as TypeScriptApi;
  }
  return createRequire(path.join(path.resolve(sourceRoot), 'package.json'))('typescript') as TypeScriptApi;
}

export function openProgram(ts: TypeScriptApi, sourceRoot: string, tsconfig: string): SourceProgram {
  const configPath = path.resolve(tsconfig);
  const read = ts.readConfigFile(configPath, ts.sys.readFile);
  if (read.error) {
    throw new Error(`cannot read ${configPath}: ${ts.flattenDiagnosticMessageText(read.error.messageText, '\n')}`);
  }
  const config = ts.parseJsonConfigFileContent(read.config, ts.sys, path.dirname(configPath));
  const host: TS.LanguageServiceHost = {
    getScriptFileNames: () => config.fileNames,
    getScriptVersion: () => '0',
    getScriptSnapshot: (file) =>
      fs.existsSync(file) && fs.statSync(file).isFile() ? ts.ScriptSnapshot.fromString(fs.readFileSync(file, 'utf8')) : undefined,
    getCurrentDirectory: () => path.dirname(configPath),
    getCompilationSettings: () => config.options,
    getDefaultLibFileName: (options) => ts.getDefaultLibFilePath(options),
    fileExists: ts.sys.fileExists,
    readFile: ts.sys.readFile,
    readDirectory: ts.sys.readDirectory,
    directoryExists: ts.sys.directoryExists,
    getDirectories: ts.sys.getDirectories,
  };
  const service = ts.createLanguageService(host, ts.createDocumentRegistry());
  const program = service.getProgram();
  if (!program) throw new Error(`the language service produced no program for ${configPath}`);
  const byPath = new Map(program.getSourceFiles().map((sf) => [normalizePath(sf.fileName), sf]));
  return {
    ts,
    root: normalizePath(sourceRoot),
    tsconfig: normalizePath(configPath),
    service,
    program,
    checker: program.getTypeChecker(),
    sourceFile: (file) => byPath.get(normalizePath(file)),
  };
}
