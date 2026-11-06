// One translation, start to end: the source program opened with its own compiler, the flow of values solved over the
// feature's file (the one owner of what each value is in C++), the units traced from the spec's roots, planned,
// emitted and laid out in the owner folder. Nothing is written to the target: the result is a map of files plus what
// is left to other owners and every error. A translation with an error is not a translation: the caller refuses it.
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';
import type { GraphIR } from '../graph/ir.ts';
import { loadTypeScript, openProgram, type SourceProgram, type TypeScriptApi } from '../graph/typescript.ts';
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
  /** Every declaration of the source the files hold, `module#name`: functions, constants and declared types. */
  readonly declared: readonly string[];
  readonly pending: readonly Pending[];
  readonly classes: readonly ApiClass[];
  readonly errors: readonly string[];
  /** Why each class member left out was left out: its class is written without it. */
  readonly dropped: readonly string[];
  /** What a whole translation left out by itself, `module#name`, with why. */
  readonly fallen: readonly (readonly [string, string])[];
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

/** A class other modules can make, and what can be asked of it: what the light check calls. */
export interface ApiClass {
  readonly source: string;
  readonly module: string;
  readonly struct: string;
  readonly made: Pick<ApiFunction, 'params' | 'context'>;
  readonly members: readonly (Pick<ApiFunction, 'source' | 'cpp' | 'params' | 'context' | 'ret'> & { readonly getter: boolean })[];
}

export interface ApiFunction {
  readonly source: string;
  /** The source file that exports it (relative to the source root). */
  readonly module: string;
  readonly cpp: string;
  readonly params: readonly { readonly name: string; readonly type: CppType; readonly byValue: boolean; readonly mutable?: boolean }[];
  /** Context parameters it takes after its own (a context pattern used, directly or through a call). */
  readonly context?: readonly { readonly type: string; readonly name: string }[];
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
  /** The program solved already (prepareTranslation), shared by the modules of one feature. */
  readonly prepared?: PreparedProgram;
}

export interface PreparedProgram {
  readonly ts: TypeScriptApi;
  readonly sp: SourceProgram;
  readonly sources: readonly TS.SourceFile[];
  readonly flow: FlowGraph;
  readonly patterns: Patterns;
  readonly rules: TargetRules;
  readonly relative: (f: TS.SourceFile) => string;
  readonly placement: ModulePlacement;
  /** The source types the target has, found by shape: the flow reads this same dictionary. */
  readonly types: TranslationSpec['types'];
  /** The names of the target's own types (classes, structs, enums). */
  readonly targetTypes: ReadonlySet<string>;
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

/**
 * What depends on the program and on what the target owns, not on the module translated: the flow of values is a fixed
 * point of the whole program, the costliest step by far, so a feature of many modules solves it once. The specs that
 * share one must agree on the target's owned concepts (functions, classes, memoizers, patterns), the only part of a
 * spec the flow reads.
 */
export function prepareTranslation(input: TranslateInput): PreparedProgram {
  const spec = resolveSpec(input.spec);
  const ts = loadTypeScript(input.sourceRoot, input.typescriptDir);
  const sp = openProgram(ts, input.sourceRoot, input.tsconfig);
  // The whole program, tests left out: every caller and every use is a fact; a test's values are not the program's.
  const sources = programFiles(sp.program, input.sourceRoot);
  const memoBodies: TS.Node[] = [];
  const visit = (n: TS.Node): void => {
    if (ts.isCallExpression(n) && ts.isIdentifier(n.expression) && spec.memoizers.includes(n.expression.text) && n.arguments[0]) memoBodies.push(n.arguments[0]);
    ts.forEachChild(n, visit);
  };
  for (const f of sources) visit(f);
  const patterns = new Patterns(ts, spec.patterns);
  patterns.alias = (name) => {
    const decl = sp.checker.getSymbolAtLocation(name)?.valueDeclaration;
    const value = decl && ts.isVariableDeclaration(decl) && (ts.getCombinedNodeFlags(decl) & ts.NodeFlags.Const) !== 0 ? decl.initializer : undefined;
    return value && ts.isPropertyAccessExpression(value) ? value : null;
  };
  const flow = new FlowGraph(ts, sp.checker, sources, spec, patterns, memoBodies);
  const rules = targetRules(input.targetRoot, spec.layer.rules);
  const target = JSON.parse(fs.readFileSync(input.targetGraph, 'utf8')) as GraphIR;
  if (target.language !== 'cpp') throw new Error(`${input.targetGraph} is not a C++ graph of the target`);
  for (const [source, found] of Object.entries(matchTargetTypes({ ts, checker: sp.checker, sources, flow, target, layers: rules.layers }))) {
    spec.types[source] = input.logic ? { ...found, type: `Logic${found.type}`, include: LOGIC_HEADER } : found;
  }
  const relative = (f: TS.SourceFile): string => path.relative(input.sourceRoot, f.fileName).split(path.sep).join('/');
  // Every file of the program placed at once: a source file keeps its owner and name whatever is translated with it.
  const placement = new ModulePlacement(sources.map(relative));
  // the target's own type names: a translated struct must not take one (city::Profile and the 4.0's Profile)
  const targetTypes = new Set(target.units.filter((u) => u.kind === 'class' || u.kind === 'enum').map((u) => u.name));
  return { ts, sp, sources, flow, patterns, rules, relative, placement, types: spec.types, targetTypes };
}

export function translate(input: TranslateInput): TranslationResult {
  const { ts, sp, sources, flow, patterns, rules, relative, placement, types: found, targetTypes } = input.prepared ?? prepareTranslation(input);
  const spec = { ...resolveSpec(input.spec), types: found };
  const file = sp.sourceFile(path.join(input.sourceRoot, spec.source));
  if (!file) throw new Error(`${spec.source} is not part of the program of ${input.tsconfig}`);
  const errors: string[] = [];
  // The flow names a shape error once: each translation starts clean, or one module's refusal would pass to the next.
  flow.errors.length = 0;
  const types = new TypeMapper(ts, sp.checker, spec, flow, errors, relative, targetTypes);
  const c: TranslateContext = { dropped: new Map(), fallen: new Map(), setters: new Map(), ts, checker: sp.checker, spec, file, sources, relative, placement, flow, types, patterns, errors, includes: new Set(),
    functions: new Map(), memos: new Map(), constants: new Map(), contextConstants: new Map(), literals: new LiteralNames(ts, sp.checker), literalConstants: new Map(),
    allowedNumbers: rules.allowed };
  // The state type is declared first, so its struct carries the target name and its fields are in order.
  // A module of pure functions has no state: its spec's state source is empty.
  if (spec.state.source) {
    const stateDecl = file.statements.find((st) => (ts.isInterfaceDeclaration(st) || ts.isTypeAliasDeclaration(st)) && st.name.text === spec.state.source);
    if (!stateDecl) throw new Error(`${spec.source} declares no type ${spec.state.source}`);
    types.structFor(sp.checker.getTypeAtLocation((stateDecl as TS.InterfaceDeclaration).name), spec.state.source);
    if (spec.state.empty) emptyDefaults(ts, file, spec.state.empty, types.structs.get(spec.state.target)!);
  }
  const translator = new Translator(c);
  const units = translator.trace();
  // the types an earlier translation wrote into a module this one shares (spec.shared): declared here too, so the
  // shared module's types header is the same for both
  for (const id of spec.shared ?? []) {
    const at = id.indexOf('#');
    const shared = sp.sourceFile(path.join(input.sourceRoot, id.slice(0, at)));
    const name = id.slice(at + 1);
    const decl = shared?.statements.find((st) => (ts.isInterfaceDeclaration(st) || ts.isTypeAliasDeclaration(st)) && st.name.text === name) as TS.InterfaceDeclaration | TS.TypeAliasDeclaration | undefined;
    if (decl) types.structFor(sp.checker.getTypeAtLocation(decl.name), name);
  }
  translator.plan(units);
  translator.emit();
  const files = translationFiles(c, translator.emitted, translator.builders);
  const listed = [
    ...[...c.functions.values()].filter((f) => !c.dropped.has(f) && !f.implicit).map((f) => ({ source: `${f.module}#${f.source}`, cpp: f.cpp, kind: (f.edit ? 'edit' : 'query') as 'edit' | 'query', line: f.line })),
    ...[...c.memos.values()].map((m) => ({ source: m.source, cpp: `${m.struct} (index)`, kind: 'index' as const, line: m.line })),
    ...[...c.constants.values()].map((k) => ({ source: k.module ? `${k.module}#${k.source}` : k.source, cpp: k.cpp, kind: 'constant' as const, line: k.line })),
  ].sort((a, b) => a.line - b.line);
  // Shapes that cannot meet in one struct are found while the types are written: they are errors too.
  const api = [...c.functions.values()].filter((f) => f.exported && !f.owner)
    .map((f) => ({ source: f.source, module: f.module, cpp: f.cpp, params: f.params, ret: f.ret, edit: f.edit !== null, ...(f.context ? { context: f.context } : {}) }));
  const declared = [...new Set([
    ...[...c.functions.values()].filter((f) => !c.dropped.has(f)).map((f) => `${f.module}#${f.source}`),
    ...[...c.constants.values()].filter((k) => k.module).map((k) => `${k.module}#${k.source}`),
    ...[...types.structs.values()].filter((s) => s.declared && s.module && !types.indexStructs.has(s.name)).map((s) => `${s.module}#${s.declared}`),
  ])].sort();
  const exportedClass = (node: TS.ClassDeclaration): boolean => (node.modifiers ?? []).some((m) => m.kind === ts.SyntaxKind.ExportKeyword) && !node.typeParameters?.length;
  const classes = new Map<TS.ClassDeclaration, ApiClass & { members: ApiClass['members'][number][] }>();
  for (const f of c.functions.values()) {
    if (!f.owner || f.owner.abstract || !exportedClass(f.owner.node as TS.ClassDeclaration) || c.dropped.has(f)) continue;
    const row = classes.get(f.owner.node) ?? { source: f.owner.source, module: f.module, struct: f.owner.struct, made: { params: [] }, members: [] };
    classes.set(f.owner.node, row);
    const name = f.source.slice(f.source.indexOf('.') + 1);
    if (f.owner.kind === 'constructor') (row as { made: ApiClass['made'] }).made = { params: f.params, ...(f.context ? { context: f.context } : {}) };
    else if (!f.owner.isStatic) row.members.push({ source: name, cpp: f.cpp, params: f.params, ret: f.ret, getter: f.owner.kind === 'getter', ...(f.context ? { context: f.context } : {}) });
  }
  return { feature: spec.feature, subsystem: spec.subsystem, files, units: listed, declared, classes: [...classes.values()], dropped: [...c.dropped.values()], fallen: [...c.fallen, ...[...c.dropped].map(([f, why]) => [`${f.module}#${f.source}`, why] as const)], pending: spec.pending,
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
