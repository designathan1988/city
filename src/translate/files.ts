// The translation's files: one module per source file, in the owner folder the placement gives it. Each module has
// `<stem>_types.h` (its enums, constants with their source, its structs; the state's module also the indexes and
// the state), `<stem>.h` (what other modules and owners call), `<stem>_edit.cpp`, `<stem>_query.cpp` and, for the
// state's module, `<stem>_index.cpp`. A header includes exactly the headers of the modules its declarations name; a
// source file, the modules its code was recorded using while it was written. Every file opens with the comment the
// target's gates require: the module's own summary (the source file's first comment).
import type * as TS from 'typescript';
import fs from 'node:fs';
import path from 'node:path';
import type { TranslateContext } from './context.ts';
import { RUNTIME_INCLUDE } from './expressions.ts';

/** The runtime of the ECMAScript built-ins (each checked against JavaScript: test/js-runtime.test.ts). */
const RUNTIME_DIR = path.join(import.meta.dirname, '..', '..', 'cpp', 'js');
/** Where the translation writes it in the target. */
const RUNTIME_ROOT = path.posix.dirname(`native/src/${RUNTIME_INCLUDE}`);
/** Its files as the target holds them, each with its text (a header of each subject: the target's file limits). */
export function runtimeFiles(): Map<string, string> {
  return new Map(fs.readdirSync(RUNTIME_DIR).sort().map((f) => [`${RUNTIME_ROOT}/${f}`, fs.readFileSync(path.join(RUNTIME_DIR, f), 'utf8')]));
}
export const isRuntimeFile = (file: string): boolean => path.posix.dirname(file) === RUNTIME_ROOT && /^js_\w+\.h$/.test(path.posix.basename(file));
import { declare, render, type CppType } from './cpptype.ts';
import type { ModuleId } from './modules.ts';
import type { CppStruct } from './types.ts';
import type { EmittedFunction, EmittedMemo } from './translator.ts';

/**
 * The standard header of each std:: name the translation writes. The headers come from the text written, the
 * module's own, not from the places that remembered to ask for one: a name written is a header included.
 */
const STD_HEADER: Readonly<Record<string, string>> = {
  optional: 'optional', nullopt: 'optional', function: 'functional', greater: 'functional', less: 'functional', vector: 'vector',
  deque: 'deque', string: 'string', to_string: 'string', unordered_map: 'unordered_map', unordered_set: 'unordered_set',
  numeric_limits: 'limits', regex: 'regex', regex_search: 'regex', regex_replace: 'regex', regex_constants: 'regex', back_inserter: 'iterator',
  get: 'tuple', array: 'array', pair: 'utility', move: 'utility', swap: 'utility', tuple: 'tuple', queue: 'queue',
  priority_queue: 'queue', initializer_list: 'initializer_list', size_t: 'cstddef', int64_t: 'cstdint', int32_t: 'cstdint', uint32_t: 'cstdint',
  ...Object.fromEntries(['sort', 'stable_sort', 'find', 'find_if', 'count_if', 'copy_if', 'any_of', 'all_of', 'none_of', 'min', 'max',
    'reverse', 'fill', 'clamp', 'lower_bound', 'upper_bound', 'remove_if'].map((n) => [n, 'algorithm'])),
  ...Object.fromEntries(['floor', 'ceil', 'round', 'trunc', 'sqrt', 'hypot', 'sin', 'cos', 'tan', 'atan', 'atan2', 'asin', 'acos', 'exp',
    'log', 'pow', 'fabs', 'fmod', 'isfinite', 'isnan', 'abs'].map((n) => [n, 'cmath'])),
};

/** The standard headers the written text names. */
function stdHeadersIn(text: string): Set<string> {
  const out = new Set<string>();
  for (const m of text.matchAll(/\bstd::(\w+)/g)) {
    const h = STD_HEADER[m[1]!];
    if (h) out.add(h);
  }
  // the fixed-width integers are written without std::
  if (/\bu?int(8|16|32|64)_t\b/.test(text)) out.add('cstdint');
  return out;
}

/** The modules of the structs and enums a text names (a whole word: `Frame`, not `FrameOut`). */
function typesNamedIn(text: string, nameModule: ReadonlyMap<string, string>): Set<string> {
  const words = new Set(text.match(/\b[A-Z]\w*\b/g) ?? []);
  const out = new Set<string>();
  for (const w of words) {
    const m = nameModule.get(w);
    if (m) out.add(m);
  }
  return out;
}

/**
 * Each target header the spec names, with the names it declares there: a value type's (V2), a class mapping's type,
 * the names a pattern writes and its context's type (Tuning). A module that writes one of them includes the header.
 */
function projectHeaderNames(c: TranslateContext): Map<string, Set<string>> {
  const out = new Map<string, Set<string>>();
  const add = (include: string, text: string): void => {
    if (!include.endsWith('.h')) return;
    const names = out.get(include) ?? new Set<string>();
    for (const w of text.replace(/\$\w+/g, ' ').replace(/std::\w+/g, ' ').match(/\b[A-Za-z_]\w*\b/g) ?? []) if (!CPP_WORDS.has(w)) names.add(w);
    out.set(include, names);
  };
  out.set(RUNTIME_INCLUDE, new Set(['js']));
  for (const t of Object.values(c.spec.types)) add(t.include, t.type);
  for (const k of Object.values(c.spec.classes)) add(k.include, k.type);
  for (const p of c.spec.patterns) add(p.include, `${p.target} ${p.context?.type ?? ''}`);
  for (const f of Object.values(c.spec.functions)) if (f.include) add(f.include, `${f.template} ${f.context?.type ?? ''}`);
  return out;
}

const CPP_WORDS = new Set(['const', 'int', 'float', 'double', 'bool', 'auto', 'unsigned', 'return', 'true', 'false', 'nullptr', 'int32_t',
  'int64_t', 'uint32_t', 'uint8_t', 'size_t']);

/** The marker the types header holds where the target's headers go. */
const PROJECT_MARK = '@@project-includes@@';

/** The marker the types header holds where its standard includes go, filled once every file is written. */
const STD_MARK = '@@std-includes@@';

function comment(text: string | null, indent = ''): string[] {
  if (!text) return [];
  const words = text.split(/\s+/);
  const lines: string[] = [];
  let line = '';
  for (const w of words) {
    if ((line + ' ' + w).length > 112 - indent.length) {
      lines.push(line);
      line = w;
    } else line = line ? `${line} ${w}` : w;
  }
  if (line) lines.push(line);
  return lines.map((l) => `${indent}// ${l}`);
}

/** The struct and enum names a type holds or refers to. */
function namesIn(t: CppType): string[] {
  switch (t.k) {
    case 'struct':
    case 'enum':
      return [t.name];
    case 'optional':
    case 'vector':
    case 'deque':
    case 'list':
    case 'set':
      return namesIn(t.of);
    case 'ptr':
      return namesIn(t.to);
    case 'map':
      return [...namesIn(t.key), ...namesIn(t.of)];
    case 'function':
      return [...t.params.flatMap(namesIn), ...namesIn(t.ret)];
    case 'lambda':
      return namesIn(t.ret);
    default:
      return [];
  }
}

/** Structs in an order where each comes after the structs it holds by value. */
function ordered(structs: readonly CppStruct[]): CppStruct[] {
  const byName = new Map(structs.map((s) => [s.name, s]));
  const held = (t: CppType): string[] => {
    switch (t.k) {
      case 'struct':
        return [t.name];
      case 'optional':
      case 'vector':
      case 'deque':
      case 'list':
        return held(t.of);
      case 'map':
        return [...held(t.key), ...held(t.of)];
      default:
        return [];
    }
  };
  const out: CppStruct[] = [];
  const seen = new Set<string>();
  const visit = (s: CppStruct): void => {
    if (seen.has(s.name)) return;
    seen.add(s.name);
    for (const f of s.fields) for (const n of held(f.type)) {
      const dep = byName.get(n);
      if (dep && dep !== s) visit(dep);
    }
    out.push(s);
  };
  for (const s of structs) visit(s);
  return out;
}

/** A source file's own summary: the first sentence of the comment it opens with. */
function summaryOf(file: TS.SourceFile, ts: TranslateContext['ts']): string | null {
  const first = file.statements[0];
  const ranges = first ? ts.getLeadingCommentRanges(file.text, first.getFullStart()) ?? [] : [];
  const text = ranges.map((r) => file.text.slice(r.pos, r.end)).join(' ').replace(/\/\*\*?|\*\/|^\s*\*|\/\//gm, ' ').replace(/\s+/g, ' ').trim();
  if (!text) return null;
  const sentence = /^(.+?[.!?])(\s|$)/.exec(text)?.[1] ?? text;
  return sentence.replace(/[.!?]$/, '');
}

const pascalIndex = (state: string): string => `${state.replace(/State$/, '')}Index`;

export function translationFiles(c: TranslateContext, functions: readonly EmittedFunction[], builders: readonly EmittedMemo[]): Map<string, string> {
  const { spec } = c;
  const ns = spec.namespace;
  const stateModule = c.relative(c.file);
  const state = spec.state.target;
  const files = new Map<string, string>();
  const sourceFiles = new Map(c.sources.map((f) => [c.relative(f), f]));
  // ---- which module everything belongs to
  const structModule = (s: { module: string | null; name: string }): string => (c.types.indexStructs.has(s.name) || s.name === state || s.module === null ? stateModule : s.module);
  const nameModule = new Map<string, string>();
  for (const s of c.types.structs.values()) nameModule.set(s.name, structModule(s));
  for (const e of c.types.enums.values()) nameModule.set(e.name, e.module ?? stateModule);
  const modules = new Set<string>([stateModule]);
  for (const f of functions) modules.add(f.plan.module);
  for (const k of c.constants.values()) modules.add(k.module ?? stateModule);
  for (const k of c.literalConstants.values()) if (k.module) modules.add(k.module);
  for (const m of nameModule.values()) modules.add(m);
  const placed = new Map<string, ModuleId>();
  for (const m of modules) {
    const id = c.placement.of(m);
    if ('error' in id) c.errors.push(id.error);
    else placed.set(m, id);
  }
  const header = (m: string, kind: 'types' | 'api'): string => {
    const id = placed.get(m)!;
    return `${id.owner.replace(/^native\/src\//, '')}/${id.stem}${kind === 'types' ? '_types' : ''}.h`;
  };
  const depsOf = (types: readonly CppType[], self: string): string[] =>
    [...new Set(types.flatMap(namesIn).map((n) => nameModule.get(n)).filter((m): m is string => m !== undefined && m !== self && placed.has(m)))].sort();

  for (const [m, id] of placed) {
    const source = sourceFiles.get(m);
    const summary = (source && summaryOf(source, c.ts)) ?? `Roadcraft 4.0 ${m}`;
    const top = (what: string): string[] => [`// ${summary}: ${what}.`, `// Translated from Roadcraft 4.0 ${m} by the converter; the owner is ${id.owner}.`];
    const isState = m === stateModule;
    const enums = [...c.types.enums.values()].filter((e) => (e.module ?? stateModule) === m);
    const constants = [...[...c.literalConstants.values()].filter((k) => k.module === m), ...[...c.constants.values()].filter((k) => (k.module ?? stateModule) === m)];
    const structs = ordered([...c.types.structs.values()].filter((s) => structModule(s) === m && s.name !== state && !c.types.indexStructs.has(s.name)));
    const memoStructs = isState ? builders : [];
    const own = functions.filter((f) => f.plan.module === m);

    // ---- types
    const typeRefs: CppType[] = [...structs.flatMap((s) => s.fields.map((f) => f.type)), ...constants.map((k) => k.type),
      ...memoStructs.flatMap((b) => b.plan.fields.map((f) => f.type))];
    const stateStruct = isState ? c.types.structs.get(state) : undefined;
    if (stateStruct) typeRefs.push(...stateStruct.fields.map((f) => f.type));
    for (const b of memoStructs) for (const owned of b.plan.stores.keys()) typeRefs.push({ k: 'struct', name: owned });
    const types: string[] = [...top('types and constants'), '#pragma once', ''];
    types.push(PROJECT_MARK);
    for (const d of depsOf(typeRefs, m)) types.push(`#include "${header(d, 'types')}"`);
    types.push('', STD_MARK, '', `namespace ${ns} {`, '');
    for (const e of enums) types.push(...comment(e.comment), `enum class ${e.name} : int { ${e.values.map((v) => c.types.enumValue(e, v).split('::')[1]).join(', ')} };`, '');
    // a constant whose type names a struct of this module comes after the structs (C++ declares before use); the
    // others before them, where a field's default may read them
    const ownStructs = new Set(structs.map((s) => s.name));
    const late = constants.filter((k) => namesIn(k.type).some((n) => ownStructs.has(n)));
    const constantLines = (ks: typeof constants): string[] => ks.flatMap((k) => [
      ...comment(`${k.doc ?? ''}${k.line > 0 ? ` (Roadcraft 4.0 ${m}:${k.line})` : ''}`.trim()),
      k.constexpr ? `constexpr ${render(k.type)} ${k.cpp} = ${k.value};` : `inline const ${render(k.type)} ${k.cpp} = ${k.value};`]);
    const early = constants.filter((k) => !late.includes(k));
    types.push(...constantLines(early));
    if (early.length > 0) types.push('');
    for (const s of structs) types.push(...structLines(s, m));
    types.push(...constantLines(late));
    if (late.length > 0) types.push('');
    for (const b of memoStructs) {
      types.push(...comment(b.plan.doc), `// (Roadcraft 4.0 ${b.plan.writtenIn}:${b.plan.line}: rebuilt by every edit)`, `struct ${b.plan.struct} {`);
      for (const [owned, member] of b.plan.stores) types.push(`\tstd::deque<${owned}> ${member}; // owned here, filed below by address (a deque never moves them)`);
      for (const f of b.plan.fields) types.push(`\t${declare(f.type, f.name)};`);
      types.push('};', '');
    }
    if (memoStructs.length > 0) {
      const index = pascalIndex(state);
      types.push(
        '// The indexes of the state: rebuilt by every edit (reindex), read by queries. They point into the state, so a',
        '// copy would point into the original: copying is refused, moving keeps every address.',
        `struct ${index} {`, ...memoStructs.map((b) => `\t${b.plan.struct} ${b.plan.member};`),
        `\t${index}() = default;`, `\t${index}(const ${index} &) = delete;`, `\t${index} &operator=(const ${index} &) = delete;`,
        `\t${index}(${index} &&) = default;`, `\t${index} &operator=(${index} &&) = default;`, '};', '');
    }
    if (stateStruct) {
      const lines = structLines(stateStruct, m);
      if (memoStructs.length > 0) lines.splice(lines.length - 2, 0, `\t${pascalIndex(state)} index; // rebuilt by every edit: never written elsewhere`);
      types.push(...lines);
    }
    types.push(`} // namespace ${ns}`, '');
    files.set(`${id.owner}/${id.stem}_types.h`, types.join('\n'));

    // ---- what other modules and owners call: the public functions, and the edits without reindex for other edits
    const visible = own.filter((f) => f.exported || f.plan.edit);
    const apiRefs = visible.flatMap((f) => [...f.plan.params.map((p) => p.type), f.plan.ret]);
    const api: string[] = [...top('what other owners call'), '#pragma once', '', `#include "${header(m, 'types')}"`];
    for (const d of depsOf(apiRefs, m)) api.push(`#include "${header(d, 'types')}"`);
    api.push('', `namespace ${ns} {`, '');
    for (const f of visible.filter((x) => x.exported)) api.push(...comment(f.plan.doc), `${f.signature};`, '');
    // a public template: its callers instantiate it, so its body is here; the modules it uses come with it
    const templates = visible.filter((x) => x.exported && x.plan.typeParams.length > 0);
    for (const f of templates) api.push(`${f.signature} {`, ...f.lines, '}', '');
    const templateUses = [...new Set(templates.flatMap((f) => [...f.uses]))].filter((u) => u !== m && placed.has(u)).sort();
    if (templateUses.length > 0) api.splice(api.indexOf(''), 0, ...templateUses.map((u) => `#include "${header(u, 'api')}"`));
    const raw = visible.filter((x) => !x.exported);
    if (raw.length > 0) {
      api.push('// The edits without the rebuild of the indexes, for other edits of the state (the public ones rebuild once).');
      for (const f of raw) api.push(`${f.signature};`);
      api.push('');
    }
    if (isState && builders.length > 0) api.push('// Rebuilds the indexes from the state (every edit does; also after loading a saved state).', `void reindex(${state} &t);`, '');
    api.push(`} // namespace ${ns}`, '');
    files.set(`${id.owner}/${id.stem}.h`, api.join('\n'));

    // ---- the code
    const cpp = (role: string, fns: readonly EmittedFunction[], extraUses: readonly string[]): string => {
      const uses = new Set([...fns.flatMap((f) => [...f.uses]), ...extraUses]);
      uses.delete(m);
      const out = [...top(role), `#include "${header(m, 'api')}"`];
      for (const u of [...uses].filter((x) => placed.has(x)).sort()) out.push(`#include "${header(u, 'api')}"`);
      // the types its text names (an internal function's parameter, a local) from modules it calls nothing of
      const named = typesNamedIn(fns.map((f) => `${f.signature}\n${f.lines.join('\n')}`).join('\n'), nameModule);
      for (const d of [...named].filter((x) => x !== m && !uses.has(x) && placed.has(x)).sort()) out.push(`#include "${header(d, 'types')}"`);
      out.push('', `namespace ${ns} {`, '');
      const internal = fns.filter((f) => !f.exported && !f.plan.edit);
      if (internal.length > 0) {
        out.push('namespace {', '', ...internal.map((f) => `${f.signature};`), '');
        for (const f of internal) out.push(...comment(f.plan.doc), `${f.signature} {`, ...f.lines, '}', '');
        out.push('} // namespace', '');
      }
      for (const f of fns.filter((x) => (x.exported || x.plan.edit) && !(x.exported && x.plan.typeParams.length > 0))) out.push(...comment(f.plan.doc), `${f.signature} {`, ...f.lines, '}', '');
      out.push(`} // namespace ${ns}`, '');
      return out.join('\n');
    };
    const edits = own.filter((f) => f.role === 'edit');
    // A public edit rebuilds the state's indexes: its module needs the state's module.
    if (edits.length > 0) files.set(`${id.owner}/${id.stem}_edit.cpp`, cpp('the edits (each rebuilds the indexes once)', edits, builders.length > 0 ? [stateModule] : []));
    const queries = own.filter((f) => f.role === 'query');
    if (queries.length > 0) files.set(`${id.owner}/${id.stem}_query.cpp`, cpp('the queries (they only read)', queries, []));
    if (isState && builders.length > 0) {
      const uses = new Set(builders.flatMap((b) => [...b.uses]));
      uses.delete(m);
      const index: string[] = [...top('the indexes of the state'), `#include "${header(m, 'api')}"`];
      for (const u of [...uses].filter((x) => placed.has(x)).sort()) index.push(`#include "${header(u, 'api')}"`);
      index.push('', `namespace ${ns} {`, '', 'namespace {', '');
      for (const b of builders) index.push(...comment(b.plan.doc), `void build_${b.plan.member}(const ${state} &t, ${b.plan.struct} &index) {`, ...b.lines, '}', '');
      index.push('} // namespace', '', `void reindex(${state} &t) {`, `\tt.index = ${pascalIndex(state)}();`);
      for (const b of builders) index.push(`\tbuild_${b.plan.member}(t, t.index.${b.plan.member});`);
      index.push('}', '', `} // namespace ${ns}`, '');
      files.set(`${id.owner}/${id.stem}_index.cpp`, index.join('\n'));
    }
  }
  // a module's types header includes what the module's own files name (each of them includes it first): the standard
  // headers of the std:: names written, the target headers of the names the spec says they declare. From the text,
  // so a module shared by two translations is written the same by both
  const owners = projectHeaderNames(c);
  for (const id of placed.values()) {
    const prefix = `${id.owner}/${id.stem}`;
    const own = [`${prefix}_types.h`, `${prefix}.h`, `${prefix}_edit.cpp`, `${prefix}_query.cpp`, `${prefix}_index.cpp`].filter((f) => files.has(f));
    const text = own.map((f) => files.get(f)!).join('\n');
    const std = stdHeadersIn(text);
    const words = new Set(text.match(/\b[A-Za-z_]\w*\b/g) ?? []);
    const project = [...owners].filter(([, names]) => [...names].some((n) => words.has(n))).map(([h]) => h).sort();
    const types = `${prefix}_types.h`;
    files.set(types, files.get(types)!
      .replace(`${PROJECT_MARK}\n`, project.map((h) => `#include "${h}"\n`).join(''))
      .replace(STD_MARK, [...std].sort().map((h) => `#include <${h}>`).join('\n')));
  }
  // the runtime of the built-ins, written beside the translation when its text calls it (core/js_runtime.h)
  if ([...files.values()].some((text) => /\bjs::/.test(text))) for (const [name, text] of runtimeFiles()) files.set(name, text);
  return files;
}

function structLines(s: CppStruct, source: string): string[] {
  const head = comment(s.comment ?? null);
  if (s.line > 0) head.push(`// (Roadcraft 4.0 ${source}:${s.line})`);
  const fields = s.fields.map((f) => `\t${declare(f.type, f.name)}${f.init};${f.comment ? ` // ${f.comment}` : ''}`);
  return [...head, `struct ${s.name} {`, ...fields, '};', ''];
}
