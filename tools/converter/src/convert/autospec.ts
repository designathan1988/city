// A translation's spec from facts: what a person wrote by hand for transit, found in the program. The units are the
// functions the ledger gave the machine; the state is the type the module's edits take and return; the empty state is
// the function that makes it from nothing; the owner comes from the module's place; what the game owns comes from the
// conversion library, written once for every feature.
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';
import type { SourceProgram, TypeScriptApi } from '../graph/typescript.ts';
import { ModulePlacement } from '../translate/modules.ts';
import { snake } from '../translate/names.ts';
import type { ClassMapping, FunctionMapping, PatternMapping, TranslationSpec } from '../translate/spec.ts';
import type { LedgerUnit } from './ledger.ts';

export interface ConversionLibrary {
  readonly namespace: string;
  readonly number: string;
  readonly stateRoot: string;
  readonly stateAnchor: string;
  readonly rules: string;
  readonly doc: string;
  readonly ownedFiles: readonly string[];
  readonly functions: Readonly<Record<string, FunctionMapping>>;
  readonly classes: Readonly<Record<string, ClassMapping>>;
  /**
   * Source types the game has (RoadStructureSpec → StructureSpec in sim/roads/road_grade.h), with their fields
   * (source name → the game's): a value of one is the game's, its fields read by these names.
   */
  readonly types?: TranslationSpec['types'];
  readonly memoizers: readonly string[];
  readonly patterns: readonly PatternMapping[];
}

export function readLibrary(file: string): ConversionLibrary {
  return JSON.parse(fs.readFileSync(file, 'utf8')) as ConversionLibrary;
}

/** The names the library owns: reached by a feature, never translated. */
export const ownedNames = (library: ConversionLibrary): string[] => [...Object.keys(library.functions), ...Object.keys(library.classes), ...library.memoizers];

interface FunctionFacts {
  readonly name: string;
  readonly node: TS.SignatureDeclaration;
}

function topLevelFunctions(ts: TypeScriptApi, file: TS.SourceFile): FunctionFacts[] {
  const out: FunctionFacts[] = [];
  for (const st of file.statements) {
    if (ts.isFunctionDeclaration(st) && st.name) out.push({ name: st.name.text, node: st });
    if (!ts.isVariableStatement(st)) continue;
    for (const d of st.declarationList.declarations) {
      if (ts.isIdentifier(d.name) && d.initializer && (ts.isArrowFunction(d.initializer) || ts.isFunctionExpression(d.initializer))) out.push({ name: d.name.text, node: d.initializer });
    }
  }
  return out;
}

const nameOf = (t: TS.Type): string | undefined => (t.aliasSymbol ?? t.getSymbol())?.name;

/** The type the module's edits take first and give back (alone, or as `data`), declared in the module; null: it has none. */
function stateType(ts: TypeScriptApi, checker: TS.TypeChecker, file: TS.SourceFile, functions: readonly FunctionFacts[]): string | null {
  const declared = new Set<string>();
  for (const st of file.statements) if (ts.isInterfaceDeclaration(st) || ts.isTypeAliasDeclaration(st)) declared.add(st.name.text);
  const votes = new Map<string, number>();
  for (const { node } of functions) {
    const first = node.parameters[0];
    const signature = checker.getSignatureFromDeclaration(node);
    if (!first || !signature) continue;
    const taken = nameOf(checker.getTypeAtLocation(first));
    if (!taken || !declared.has(taken)) continue;
    const ret = signature.getReturnType();
    const data = ret.getProperty('data');
    if (nameOf(ret) === taken || (data && nameOf(checker.getTypeOfSymbol(data)) === taken)) votes.set(taken, (votes.get(taken) ?? 0) + 1);
  }
  return [...votes].sort((a, b) => b[1] - a[1] || a[0].localeCompare(b[0]))[0]?.[0] ?? null;
}

/** The constant arrow function that makes the state from nothing. */
function emptyMaker(ts: TypeScriptApi, checker: TS.TypeChecker, functions: readonly FunctionFacts[], state: string): string | undefined {
  return functions.find(({ node }) => {
    const signature = checker.getSignatureFromDeclaration(node);
    return ts.isArrowFunction(node) && node.parameters.length === 0 && signature !== undefined && nameOf(signature.getReturnType()) === state;
  })?.name;
}

/** The module's first comment, its first sentence. */
function summaryOf(ts: TypeScriptApi, file: TS.SourceFile): string {
  const first = ts.getLeadingCommentRanges(file.text, 0)?.[0];
  const text = first ? file.text.slice(first.pos, first.end).replace(/\/\*\*?|\*\/|^\s*\*|\/\//gm, ' ').replace(/\s+/g, ' ').trim() : '';
  return text.split(/(?<=\.)\s/)[0] || path.basename(file.fileName);
}

/** One spec per source module with functions the ledger gave the machine; an empty state source: a module of pure functions. */
export function moduleSpecs(ts: TypeScriptApi, sp: SourceProgram, sourceRoot: string, feature: string, units: readonly LedgerUnit[], library: ConversionLibrary, sources: readonly string[]): TranslationSpec[] {
  const placement = new ModulePlacement(sources);
  const byFile = new Map<string, LedgerUnit[]>();
  for (const u of units) {
    if (u.disposition !== 'translate' || (u.kind !== 'function' && u.kind !== 'variable' && u.kind !== 'class')) continue;
    const list = byFile.get(u.file);
    if (list) list.push(u);
    else byFile.set(u.file, [u]);
  }
  const out: TranslationSpec[] = [];
  for (const [module, roots] of [...byFile].sort(([a], [b]) => a.localeCompare(b))) {
    const file = sp.sourceFile(path.join(sourceRoot, module));
    const placed = placement.of(module);
    if (!file || 'error' in placed) continue;
    const functions = topLevelFunctions(ts, file);
    // what the ledger gave the machine: its functions and its constants, each a root of its own
    const known = new Set(functions.map((f) => f.name));
    for (const st of file.statements) {
      if (ts.isClassDeclaration(st) && st.name) known.add(st.name.text);
      if (ts.isVariableStatement(st)) for (const d of st.declarationList.declarations) if (ts.isIdentifier(d.name)) known.add(d.name.text);
    }
    const names = roots.map((r) => r.id.slice(r.id.indexOf('#') + 1)).filter((n) => known.has(n));
    if (names.length === 0) continue;
    // the state is what the module makes from nothing: a pure function of a value (expand(box): Aabb) edits no state, and
    // written in place it would change what the caller keeps
    const candidate = stateType(ts, sp.checker, file, functions);
    const empty = candidate ? emptyMaker(ts, sp.checker, functions, candidate) : undefined;
    const state = empty ? candidate : null;
    const target = state ? `${state.replace(/(Data|Doc|State)$/, '')}State` : '';
    const member = snake(target.replace(/State$/, ''));
    const relative = placed.owner.replace(/^native\/src\//, '');
    const summary = summaryOf(ts, file);
    out.push({
      feature, subsystem: placed.owner, namespace: library.namespace, source: module, summary,
      state: { source: state ?? '', target, member, root: library.stateRoot, ...(empty ? { empty } : {}), anchor: library.stateAnchor, memberLine: `${target} ${member}; // ${relative}` },
      registry: { doc: library.doc, mapLine: `- \`${placed.owner}/**\` — ${summary}` },
      units: names, pending: [], number: library.number, types: { ...(library.types ?? {}) }, functions: library.functions, classes: library.classes,
      memoizers: library.memoizers, patterns: library.patterns, typeNames: {},
      layer: { rules: library.rules, includes: ['core/', `${relative}/`] },
    });
  }
  return out;
}
