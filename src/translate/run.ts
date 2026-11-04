// One translation, start to end: the source program opened with its own compiler, the flow of values solved over the
// feature's file (the one owner of what each value is in C++), the units traced from the spec's roots, planned,
// emitted and laid out in the owner folder. Nothing is written to the target: the result is a map of files plus what
// is left to other owners and every error. A translation with an error is not a translation: the caller refuses it.
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';
import type { GraphIR } from '../graph/ir.ts';
import { loadTypeScript, openProgram } from '../graph/typescript.ts';
import { matchTargetTypes } from './target-types.ts';
import { LiteralNames } from './literal-names.ts';
import type { TranslateContext } from './context.ts';
import type { CppType } from './cpptype.ts';
import { translationFiles } from './files.ts';
import { ModulePlacement } from './modules.ts';
import { FlowGraph } from './flow.ts';
import { Patterns } from './patterns.ts';
import { resolveSpec, type Pending, type TranslationSpec } from './spec.ts';
import { Translator } from './translator.ts';
import { TypeMapper, type CppEnum, type CppStruct } from './types.ts';

export interface TranslationResult {
  readonly feature: string;
  readonly subsystem: string;
  readonly files: Map<string, string>;
  readonly units: { readonly source: string; readonly cpp: string; readonly kind: 'edit' | 'query' | 'index' | 'constant'; readonly line: number }[];
  readonly pending: readonly Pending[];
  readonly errors: readonly string[];
  /** What other owners call, and the types they see: what a harness needs to drive the translation. */
  readonly api: readonly ApiFunction[];
  readonly structs: readonly CppStruct[];
  readonly enums: readonly CppEnum[];
  readonly state: string;
  /** The value types the target has, as found: C++ type → its fields (source name → target name). */
  readonly valueTypes: Readonly<Record<string, Readonly<Record<string, string>>>>;
  /** The source file that declares the state (its module holds the state, its indexes and its empty value). */
  readonly stateModule: string;
  /** The header the state manager includes to hold the state (the state module's types header). */
  readonly stateHeader: string;
  /** The source files the translation read (the feature's file and the modules it imports), relative to the root. */
  readonly sourcesRead: readonly string[];
}

export interface ApiFunction {
  readonly source: string;
  /** The source file that exports it (relative to the source root). */
  readonly module: string;
  readonly cpp: string;
  readonly params: readonly { readonly name: string; readonly type: CppType; readonly byValue: boolean }[];
  readonly ret: CppType;
  /** An edit changes the state (its first parameter); `ret` is what it returns besides. */
  readonly edit: boolean;
}

export interface TranslateInput {
  readonly sourceRoot: string;
  readonly tsconfig: string;
  readonly typescriptDir: string | undefined;
  readonly spec: TranslationSpec;
  readonly targetRoot: string;
  /** The target's C++ graph (`converter cppgraph`): the types the target already has. */
  readonly targetGraph: string;
  /** The differential's logic build: each target value type found is written as its own double-field type. */
  readonly logic?: boolean;
}

/** The logic build's header of value types: what `translate({ logic: true })` maps the source's value types to. */
export const LOGIC_HEADER = 'converter_logic_values.h';

interface TargetRules {
  readonly allowed: number[];
  readonly layers: Record<string, string[]>;
}

/** What the target's gate rules say: the numbers allowed bare, and what each folder may include. */
function targetRules(targetRoot: string, rulesFile: string): TargetRules {
  const rules = JSON.parse(fs.readFileSync(path.join(targetRoot, rulesFile), 'utf8')) as { magic_numbers?: { allowed_values?: number[] }; cpp_layers?: Record<string, string[]> };
  const allowed = rules.magic_numbers?.allowed_values;
  if (!Array.isArray(allowed)) throw new Error(`${rulesFile} has no magic_numbers.allowed_values`);
  if (!rules.cpp_layers) throw new Error(`${rulesFile} has no cpp_layers`);
  return { allowed, layers: rules.cpp_layers };
}

export function translate(input: TranslateInput): TranslationResult {
  const spec = resolveSpec(input.spec);
  const ts = loadTypeScript(input.sourceRoot, input.typescriptDir);
  const sp = openProgram(ts, input.sourceRoot, input.tsconfig);
  const file = sp.sourceFile(path.join(input.sourceRoot, spec.source));
  if (!file) throw new Error(`${spec.source} is not part of the program of ${input.tsconfig}`);
  const errors: string[] = [];
  // The whole program, tests left out: every caller and every use is a fact; a test's values are not the program's.
  const sources = programFiles(sp.program, input.sourceRoot);
  const memoBodies: TS.Node[] = [];
  const visit = (n: TS.Node): void => {
    if (ts.isCallExpression(n) && ts.isIdentifier(n.expression) && spec.memoizers.includes(n.expression.text) && n.arguments[0]) memoBodies.push(n.arguments[0]);
    ts.forEachChild(n, visit);
  };
  for (const f of sources) visit(f);
  const patterns = new Patterns(ts, spec.patterns);
  const flow = new FlowGraph(ts, sp.checker, sources, spec, patterns, memoBodies);
  const rules = targetRules(input.targetRoot, spec.layer.rules);
  const target = JSON.parse(fs.readFileSync(input.targetGraph, 'utf8')) as GraphIR;
  if (target.language !== 'cpp') throw new Error(`${input.targetGraph} is not a C++ graph of the target`);
  for (const [source, found] of Object.entries(matchTargetTypes({ ts, checker: sp.checker, sources, flow, target, layers: rules.layers }))) {
    spec.types[source] = input.logic ? { ...found, type: `Logic${found.type}`, include: LOGIC_HEADER } : found;
  }
  const relative = (f: TS.SourceFile): string => path.relative(input.sourceRoot, f.fileName).split(path.sep).join('/');
  const types = new TypeMapper(ts, sp.checker, spec, flow, errors, relative);
  // Every file of the program placed at once: a source file keeps its owner and name whatever is translated with it.
  const placement = new ModulePlacement(sources.map(relative));
  const c: TranslateContext = { ts, checker: sp.checker, spec, file, sources, relative, placement, flow, types, patterns, errors, includes: new Set(),
    functions: new Map(), memos: new Map(), constants: new Map(), literals: new LiteralNames(ts, sp.checker), literalConstants: new Map(),
    allowedNumbers: rules.allowed };
  // The state type is declared first, so its struct carries the target name and its fields are in order.
  const stateDecl = file.statements.find((st) => (ts.isInterfaceDeclaration(st) || ts.isTypeAliasDeclaration(st)) && st.name.text === spec.state.source);
  if (!stateDecl) throw new Error(`${spec.source} declares no type ${spec.state.source}`);
  types.structFor(sp.checker.getTypeAtLocation((stateDecl as TS.InterfaceDeclaration).name), spec.state.source);
  if (spec.state.empty) emptyDefaults(ts, file, spec.state.empty, types.structs.get(spec.state.target)!);
  const translator = new Translator(c);
  const units = translator.trace();
  translator.plan(units);
  translator.emit();
  const files = translationFiles(c, translator.emitted, translator.builders);
  const listed = [
    ...[...c.functions.values()].map((f) => ({ source: `${f.module}#${f.source}`, cpp: f.cpp, kind: (f.edit ? 'edit' : 'query') as 'edit' | 'query', line: f.line })),
    ...[...c.memos.values()].map((m) => ({ source: m.source, cpp: `${m.struct} (index)`, kind: 'index' as const, line: m.line })),
    ...[...c.constants.values()].map((k) => ({ source: k.source, cpp: k.cpp, kind: 'constant' as const, line: k.line })),
  ].sort((a, b) => a.line - b.line);
  // Shapes that cannot meet in one struct are found while the types are written: they are errors too.
  const api = [...c.functions.values()].filter((f) => f.exported)
    .map((f) => ({ source: f.source, module: f.module, cpp: f.cpp, params: f.params, ret: f.ret, edit: f.edit !== null }));
  return { feature: spec.feature, subsystem: spec.subsystem, files, units: listed, pending: spec.pending,
    errors: [...new Set([...flow.errors, ...errors])], api, structs: [...types.structs.values()].filter((s) => !types.indexStructs.has(s.name)),
    enums: [...types.enums.values()], state: spec.state.target, stateModule: relative(file),
    valueTypes: Object.fromEntries(Object.values(spec.types).filter((t) => t.fields).map((t) => [t.type, t.fields!])),
    stateHeader: stateHeader(placement, relative(file)), sourcesRead: sourcesRead(c) };
}

/** The program's own source files: under the source root, not declarations, not dependencies, not tests. */
function programFiles(program: TS.Program, sourceRoot: string): TS.SourceFile[] {
  const root = path.resolve(sourceRoot).split(path.sep).join('/').toLowerCase();
  const test = /(^|\/)(tests?|__tests__|spec)\/|\.(test|spec)\.tsx?$/;
  return program.getSourceFiles().filter((f) => {
    const name = f.fileName.split(path.sep).join('/');
    const relative = name.toLowerCase().startsWith(root + '/') ? name.slice(root.length + 1) : null;
    return relative !== null && !f.isDeclarationFile && !relative.startsWith('node_modules/') && !test.test(relative);
  });
}

function stateHeader(placement: ModulePlacement, module: string): string {
  const id = placement.of(module);
  return 'error' in id ? '' : `${id.owner.replace(/^native\/src\//, '')}/${id.stem}_types.h`;
}

/** What the translation read: every module it translated, and the files the target's owners stand in for. */
function sourcesRead(c: TranslateContext): string[] {
  const out = new Set([c.relative(c.file), ...[...c.functions.values()].map((f) => f.module)]);
  for (const k of c.constants.values()) if (k.module) out.add(k.module);
  for (const s of c.types.structs.values()) if (s.module) out.add(s.module);
  for (const e of c.types.enums.values()) if (e.module) out.add(e.module);
  return [...out].sort();
}

/** The state's defaults are the empty state's values (`emptyTransit()` starts nextId at 1). */
function emptyDefaults(ts: typeof TS, file: TS.SourceFile, name: string, state: { fields: { source: string; init: string }[] }): void {
  let literal: TS.ObjectLiteralExpression | undefined;
  const visit = (n: TS.Node): void => {
    if (ts.isVariableDeclaration(n) && ts.isIdentifier(n.name) && n.name.text === name && n.initializer && ts.isArrowFunction(n.initializer)) {
      let body: TS.Node = n.initializer.body;
      while (ts.isParenthesizedExpression(body)) body = body.expression;
      if (ts.isObjectLiteralExpression(body)) literal = body;
    }
    ts.forEachChild(n, visit);
  };
  visit(file);
  if (!literal) throw new Error(`${name} does not return an object literal: the empty state's values are unknown`);
  for (const p of literal.properties) {
    if (!ts.isPropertyAssignment(p)) continue;
    const field = state.fields.find((f) => f.source === p.name.getText());
    const v = p.initializer;
    if (field && (ts.isNumericLiteral(v) || v.kind === ts.SyntaxKind.TrueKeyword || v.kind === ts.SyntaxKind.FalseKeyword)) field.init = ` = ${v.getText()}`;
  }
}

export function renderTranslation(r: TranslationResult): string {
  const lines = [`# Translation: ${r.feature}`, '', `Owner: \`${r.subsystem}\`. ${r.errors.length === 0 ? 'Complete: no untranslated construct.' : `**Refused: ${r.errors.length} error(s).**`}`, ''];
  if (r.errors.length > 0) lines.push('## Errors', '', ...r.errors.map((e) => `- ${e}`), '');
  lines.push('## Units', '', '| Source | C++ | Kind | Line |', '|---|---|---|---|', ...r.units.map((u) => `| \`${u.source}\` | \`${u.cpp}\` | ${u.kind} | ${u.line} |`), '');
  lines.push('## Files', '', ...[...r.files.keys()].map((f) => `- \`${f}\` (${r.files.get(f)!.split('\n').length} lines)`), '');
  if (r.pending.length > 0) {
    lines.push('## Left to other owners', '', 'The package is not complete until each of these owners provides its part.', '',
      ...r.pending.map((p) => `- \`${p.unit}\` → \`${p.owner}\`: ${p.reason}`), '');
  }
  return lines.join('\n');
}
