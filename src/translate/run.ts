// One translation, start to end: the source program opened with its own compiler, the flow of values solved over the
// feature's file (the one owner of what each value is in C++), the units traced from the spec's roots, planned,
// emitted and laid out in the owner folder. Nothing is written to the target: the result is a map of files plus what
// is left to other owners and every error. A translation with an error is not a translation: the caller refuses it.
import fs from 'node:fs';
import path from 'node:path';
import type * as TS from 'typescript';
import { loadTypeScript, openProgram } from '../graph/typescript.ts';
import type { ConstantPlan, TranslateContext } from './context.ts';
import { INT, realLiteral, type CppType } from './cpptype.ts';
import { featureFiles } from './files.ts';
import { FlowGraph, Num } from './flow.ts';
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
  /** The source files the translation read (the feature's file and the modules it imports), relative to the root. */
  readonly sourcesRead: readonly string[];
}

export interface ApiFunction {
  readonly source: string;
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
}

/** The numbers the target's gates allow bare (magic_numbers.allowed_values of its rules). */
function allowedNumbers(targetRoot: string, rulesFile: string): number[] {
  const rules = JSON.parse(fs.readFileSync(path.join(targetRoot, rulesFile), 'utf8')) as { magic_numbers?: { allowed_values?: number[] } };
  const allowed = rules.magic_numbers?.allowed_values;
  if (!Array.isArray(allowed)) throw new Error(`${rulesFile} has no magic_numbers.allowed_values`);
  return allowed;
}

export function translate(input: TranslateInput): TranslationResult {
  const spec = resolveSpec(input.spec);
  const ts = loadTypeScript(input.sourceRoot, input.typescriptDir);
  const sp = openProgram(ts, input.sourceRoot, input.tsconfig);
  const file = sp.sourceFile(path.join(input.sourceRoot, spec.source));
  if (!file) throw new Error(`${spec.source} is not part of the program of ${input.tsconfig}`);
  const errors: string[] = [];
  const memoBodies: TS.Node[] = [];
  const visit = (n: TS.Node): void => {
    if (ts.isCallExpression(n) && ts.isIdentifier(n.expression) && spec.memoizers.includes(n.expression.text) && n.arguments[0]) memoBodies.push(n.arguments[0]);
    ts.forEachChild(n, visit);
  };
  visit(file);
  const patterns = new Patterns(ts, spec.patterns);
  const flow = new FlowGraph(ts, sp.checker, file, spec, patterns, memoBodies);
  const types = new TypeMapper(ts, sp.checker, spec, flow, errors);
  const real = types.number(Num.Float, 'local');
  const namedLiterals = new Map<number, ConstantPlan>(Object.entries(spec.literals).map(([value, named]) => [Number(value), {
    source: value, cpp: named.name, type: Number.isInteger(Number(value)) ? INT : real,
    value: Number.isInteger(Number(value)) ? value : realLiteral(value, spec.number), line: 0, doc: named.comment, constexpr: true,
  }]));
  const c: TranslateContext = { ts, checker: sp.checker, spec, file, flow, types, patterns, errors, includes: new Set(),
    functions: new Map(), memos: new Map(), constants: new Map(), namedLiterals,
    allowedNumbers: allowedNumbers(input.targetRoot, spec.layer.rules) };
  // The state type is declared first, so its struct carries the target name and its fields are in order.
  const stateDecl = file.statements.find((st) => (ts.isInterfaceDeclaration(st) || ts.isTypeAliasDeclaration(st)) && st.name.text === spec.state.source);
  if (!stateDecl) throw new Error(`${spec.source} declares no type ${spec.state.source}`);
  types.structFor(sp.checker.getTypeAtLocation((stateDecl as TS.InterfaceDeclaration).name), spec.state.source);
  if (spec.state.empty) emptyDefaults(ts, file, spec.state.empty, types.structs.get(spec.state.target)!);
  const translator = new Translator(c);
  const units = translator.trace();
  translator.plan(units);
  translator.emit();
  const { files } = featureFiles(c, translator.emitted, translator.builders);
  const listed = [
    ...[...c.functions.values()].map((f) => ({ source: f.source, cpp: f.cpp, kind: (f.edit ? 'edit' : 'query') as 'edit' | 'query', line: f.line })),
    ...[...c.memos.values()].map((m) => ({ source: m.source, cpp: `${m.struct} (index)`, kind: 'index' as const, line: m.line })),
    ...[...c.constants.values()].map((k) => ({ source: k.source, cpp: k.cpp, kind: 'constant' as const, line: k.line })),
  ].sort((a, b) => a.line - b.line);
  // Shapes that cannot meet in one struct are found while the types are written: they are errors too.
  const api = [...c.functions.values()].filter((f) => f.exported)
    .map((f) => ({ source: f.source, cpp: f.cpp, params: f.params, ret: f.ret, edit: f.edit !== null }));
  return { feature: spec.feature, subsystem: spec.subsystem, files, units: listed, pending: spec.pending,
    errors: [...new Set([...flow.errors, ...errors])], api, structs: [...types.structs.values()].filter((s) => !types.indexStructs.has(s.name)),
    enums: [...types.enums.values()], state: spec.state.target, sourcesRead: sourcesRead(ts, sp.checker, file, input.sourceRoot) };
}

/** The feature's file and every module it imports (what the translation read), relative to the source root. */
function sourcesRead(ts: typeof TS, checker: TS.TypeChecker, file: TS.SourceFile, root: string): string[] {
  const relative = (f: string): string => path.relative(root, f).split(path.sep).join('/');
  const out = new Set([relative(file.fileName)]);
  for (const st of file.statements) {
    if (!ts.isImportDeclaration(st)) continue;
    const module = checker.getSymbolAtLocation(st.moduleSpecifier);
    const decl = module?.valueDeclaration ?? module?.declarations?.[0];
    if (decl) out.add(relative(decl.getSourceFile().fileName));
  }
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
