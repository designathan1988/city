// The feature's files in its owner folder, by role: `<f>_types.h` (enums, constants with their source, the structs,
// the indexes and the state), `<f>.h` (what other owners may call), `<f>_edit.cpp` (the edits), `<f>_query.cpp`
// (the queries), `<f>_index.cpp` (the index builders and reindex). Every file opens with the comment the target's
// gates require. Structs come after the structs they hold.
import type { TranslateContext } from './context.ts';
import { declare, render, type CppType } from './cpptype.ts';
import type { CppStruct } from './types.ts';
import type { EmittedFunction, EmittedMemo } from './translator.ts';

export interface FeatureFiles {
  /** Path relative to the target root → contents. */
  readonly files: Map<string, string>;
  readonly typesHeader: string;
}

const STD = new Set(['algorithm', 'cmath', 'cstdint', 'deque', 'functional', 'limits', 'optional', 'queue', 'string', 'tuple',
  'unordered_map', 'unordered_set', 'utility', 'vector', 'iterator']);

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

/** Structs in an order where each comes after the structs its fields hold. */
function ordered(structs: readonly CppStruct[]): CppStruct[] {
  const byName = new Map(structs.map((s) => [s.name, s]));
  const out: CppStruct[] = [];
  const seen = new Set<string>();
  // A struct a field holds by value (directly, in an optional or a container) must be complete before it.
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
  const visit = (s: CppStruct): void => {
    if (seen.has(s.name)) return;
    seen.add(s.name);
    for (const f of s.fields) {
      for (const n of held(f.type)) {
        const dep = byName.get(n);
        if (dep && dep !== s) visit(dep);
      }
    }
    out.push(s);
  };
  for (const s of structs) visit(s);
  return out;
}

function struct(s: CppStruct, source: string): string[] {
  const head = comment(s.comment ?? null);
  if (s.line > 0) head.push(`// (Roadcraft 4.0 ${source}:${s.line})`);
  const fields = s.fields.map((f) => `\t${declare(f.type, f.name)}${f.init};${f.comment ? ` // ${f.comment}` : ''}`);
  return [...head, `struct ${s.name} {`, ...fields, '};', ''];
}

export function featureFiles(c: TranslateContext, functions: readonly EmittedFunction[], builders: readonly EmittedMemo[]): FeatureFiles {
  const { spec } = c;
  const base = `${spec.subsystem}/${spec.feature}`;
  const include = (file: string): string => `${spec.subsystem.replace(/^native\/src\//, '')}/${file}`;
  const typesHeader = `${base}_types.h`;
  const includes = [...new Set([...c.includes, ...c.types.includes])];
  if (builders.some((b) => b.plan.stores.size > 0)) includes.push('deque');
  const project = includes.filter((i) => i.endsWith('.h')).sort();
  const std = includes.filter((i) => STD.has(i)).sort();
  const header = (what: string): string[] => [`// ${spec.summary}: ${what}.`, `// Translated from Roadcraft 4.0 ${spec.source} by the converter; the owner is ${spec.subsystem}.`];

  // ---- types
  const types: string[] = [...header('types, constants and the state'), '#pragma once', ''];
  for (const p of project) types.push(`#include "${p}"`);
  if (project.length > 0) types.push('');
  for (const s of [...new Set(std)]) types.push(`#include <${s}>`);
  types.push('', `namespace ${spec.namespace} {`, '');
  for (const e of c.types.enums.values()) {
    types.push(...comment(e.comment), `enum class ${e.name} : int { ${e.values.map((v) => c.types.enumValue(e, v).split('::')[1]).join(', ')} };`, '');
  }
  const constants = [...c.namedLiterals.values(), ...c.constants.values()];
  for (const k of constants) {
    const source = k.line > 0 ? ` (Roadcraft 4.0 ${spec.source}:${k.line})` : '';
    types.push(...comment(`${k.doc ?? ''}${source}`.trim()));
    types.push(k.constexpr ? `constexpr ${render(k.type)} ${k.cpp} = ${k.value};` : `inline const ${render(k.type)} ${k.cpp} = ${k.value};`);
  }
  if (constants.length > 0) types.push('');
  const state = spec.state.target;
  const regular = ordered([...c.types.structs.values()].filter((s) => s.name !== state && !c.types.indexStructs.has(s.name)));
  for (const s of regular) types.push(...struct(s, spec.source));
  for (const b of builders) {
    types.push(...comment(b.plan.doc), `// (Roadcraft 4.0 ${spec.source}:${b.plan.line}: rebuilt by every edit)`, `struct ${b.plan.struct} {`);
    for (const [owned, member] of b.plan.stores) types.push(`\tstd::deque<${owned}> ${member}; // owned here, filed below by address (a deque never moves them)`);
    for (const f of b.plan.fields) types.push(`\t${declare(f.type, f.name)};`);
    types.push('};', '');
  }
  if (builders.length > 0) {
    types.push(
      `// The indexes of the state: rebuilt by every edit (reindex), read by queries. They point into the state, so a`,
      `// copy would point into the original: copying is refused, moving keeps every address.`,
      `struct ${pascalIndex(state)} {`,
      ...builders.map((b) => `\t${b.plan.struct} ${b.plan.member};`),
      `\t${pascalIndex(state)}() = default;`,
      `\t${pascalIndex(state)}(const ${pascalIndex(state)} &) = delete;`,
      `\t${pascalIndex(state)} &operator=(const ${pascalIndex(state)} &) = delete;`,
      `\t${pascalIndex(state)}(${pascalIndex(state)} &&) = default;`,
      `\t${pascalIndex(state)} &operator=(${pascalIndex(state)} &&) = default;`,
      '};', '');
  }
  const stateStruct = c.types.structs.get(state);
  if (stateStruct) {
    const s = { ...stateStruct, fields: [...stateStruct.fields] };
    const lines = struct(s, spec.source);
    if (builders.length > 0) lines.splice(lines.length - 2, 0, `\t${pascalIndex(state)} index; // rebuilt by every edit: never written elsewhere`);
    types.push(...lines);
  }
  types.push(`} // namespace ${spec.namespace}`, '');

  // ---- public API
  const api: string[] = [...header('what other owners call'), '#pragma once', '', `#include "${include(`${spec.feature}_types.h`)}"`, '', `namespace ${spec.namespace} {`, ''];
  for (const f of functions.filter((x) => x.exported)) api.push(...comment(f.plan.doc), `${f.signature};`, '');
  if (builders.length > 0) api.push('// Rebuilds the indexes from the state (every edit does; also after loading a saved state).', `void reindex(${state} &t);`, '');
  api.push(`} // namespace ${spec.namespace}`, '');

  const cpp = (role: string, fns: readonly EmittedFunction[], extra: string[] = []): string[] => {
    const internal = fns.filter((f) => !f.exported);
    const out = [...header(role), `#include "${include(`${spec.feature}.h`)}"`, '', `namespace ${spec.namespace} {`, ''];
    if (internal.length > 0) {
      out.push('namespace {', '');
      for (const f of internal) out.push(`${f.signature};`);
      out.push('');
      for (const f of internal) out.push(...comment(f.plan.doc), `${f.signature} {`, ...f.lines, '}', '');
      out.push('} // namespace', '');
    }
    for (const f of fns.filter((x) => x.exported)) out.push(...comment(f.plan.doc), `${f.signature} {`, ...f.lines, '}', '');
    out.push(...extra, `} // namespace ${spec.namespace}`, '');
    return out;
  };
  const files = new Map<string, string>();
  files.set(typesHeader, types.join('\n'));
  files.set(`${base}.h`, api.join('\n'));
  const edits = functions.filter((f) => f.role === 'edit');
  if (edits.length > 0) files.set(`${base}_edit.cpp`, cpp('the edits (each rebuilds the indexes once)', edits).join('\n'));
  const queries = functions.filter((f) => f.role === 'query');
  if (queries.length > 0) files.set(`${base}_query.cpp`, cpp('the queries (they only read)', queries).join('\n'));
  if (builders.length > 0) {
    const index: string[] = [...header('the indexes of the state'), `#include "${include(`${spec.feature}.h`)}"`, '', `namespace ${spec.namespace} {`, '', 'namespace {', ''];
    for (const b of builders) {
      index.push(...comment(b.plan.doc), `void build_${b.plan.member}(const ${state} &t, ${b.plan.struct} &index) {`, ...b.lines, '}', '');
    }
    index.push('} // namespace', '', `void reindex(${state} &t) {`, `\tt.index = ${pascalIndex(state)}();`);
    for (const b of builders) index.push(`\tbuild_${b.plan.member}(t, t.index.${b.plan.member});`);
    index.push('}', '', `} // namespace ${spec.namespace}`, '');
    files.set(`${base}_index.cpp`, index.join('\n'));
  }
  return { files, typesHeader };
}

function pascalIndex(state: string): string {
  return `${state.replace(/State$/, '')}Index`;
}
