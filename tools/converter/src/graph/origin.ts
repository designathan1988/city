// Names where an out-of-scope declaration comes from (a package, a TypeScript standard library, or project files
// outside the analysed scope), so every external API the source touches gets one stable key in the closed-world
// catalog that the rule stage must cover completely.
import type * as TS from 'typescript';
import { normalizePath, relativePath } from '../common/paths.ts';
import type { TypeScriptApi } from './typescript.ts';

const LIB_FILE = /\/lib\.([a-z0-9.]+)\.d\.ts$/i;

export function moduleOf(file: string, root: string): string {
  const normal = normalizePath(file);
  const lib = LIB_FILE.exec(normal);
  if (lib && /\/typescript\/lib\//.test(normal)) return libraryName(lib[1]!);
  const marker = normal.lastIndexOf('/node_modules/');
  if (marker >= 0) {
    const parts = normal.slice(marker + '/node_modules/'.length).split('/');
    const name = parts[0]!.startsWith('@') ? `${parts[0]}/${parts[1]}` : parts[0]!;
    // @types/foo describes foo; @types/scope__pkg describes @scope/pkg.
    if (name.startsWith('@types/')) return name.slice('@types/'.length).replace(/^([^_]+)__/, '@$1/');
    return name;
  }
  if (lib) return libraryName(lib[1]!);
  return 'project:' + relativePath(root, normal);
}

function libraryName(lib: string): string {
  const name = lib.toLowerCase();
  if (name.startsWith('dom')) return 'dom';
  if (name.startsWith('webworker')) return 'webworker';
  if (name.startsWith('scripthost')) return 'scripthost';
  return 'es';
}

/** The class, interface, enum, namespace or named type literal that declares `decl`, if any. */
export function containerName(ts: TypeScriptApi, decl: TS.Node): string | null {
  for (let node = decl.parent; node && !ts.isSourceFile(node); node = node.parent) {
    if (ts.isModuleDeclaration(node)) return ts.isIdentifier(node.name) ? node.name.text : null;
    if ((ts.isClassLike(node) || ts.isInterfaceDeclaration(node) || ts.isEnumDeclaration(node)) && node.name) {
      return node.name.text;
    }
    if (ts.isTypeLiteralNode(node)) {
      const owner = node.parent;
      if (owner && (ts.isTypeAliasDeclaration(owner) || ts.isVariableDeclaration(owner) || ts.isPropertySignature(owner))) {
        return ts.isIdentifier(owner.name) ? owner.name.text : null;
      }
    }
  }
  return null;
}

export function declarationName(ts: TypeScriptApi, decl: TS.Declaration, fallback: string): string {
  const name = ts.getNameOfDeclaration(decl);
  if (name && (ts.isIdentifier(name) || ts.isPrivateIdentifier(name) || ts.isStringLiteral(name))) return name.text;
  return fallback;
}

export function externalKey(module: string, container: string | null, symbol: string): string {
  return `${module}:${container ? container + '.' : ''}${symbol}`;
}

/** Keys whose value differs between runs; the trace stage replaces each with a recorded stream. */
const NONDETERMINISTIC_SUFFIXES = [
  ':Math.random',
  ':Performance.now',
  ':DateConstructor.now',
  ':Crypto.getRandomValues',
  ':Crypto.randomUUID',
  ':requestAnimationFrame',
  ':setTimeout',
  ':setInterval',
  ':queueMicrotask',
];

export function isNondeterministic(key: string): boolean {
  return NONDETERMINISTIC_SUFFIXES.some((suffix) => key.endsWith(suffix));
}
