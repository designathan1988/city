// Differential testing of a translation against its source: the same random scenario (calls of the public API with
// generated arguments) is written once in JavaScript, run on the source itself (Node runs its TypeScript, its path
// aliases resolved by a module resolve hook), and once in C++, run on the translation; both print one JSON line
// per step, normalized the same way (keys sorted; absent, null and false are one; maps as pairs in insertion order, as both keep it;
// non-finite numbers as null), and the lines are compared. Inputs are multiples of 0.5, exact in float and double,
// so a build whose numbers are doubles (JavaScript's own) must agree to the last bit but for the libraries' ulp; the
// float build is compared within float precision. Nothing is written outside the work folder.
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import { pathToFileURL } from 'node:url';
import type { TypeScriptApi } from '../graph/typescript.ts';
import { realLiteral, render, type CppType } from './cpptype.ts';
import { upper } from './names.ts';
import type { ApiFunction, TranslationResult } from './run.ts';
import type { CppEnum, CppStruct } from './types.ts';
import { RUNTIME_INCLUDE } from './expressions.ts';
import { runtimeFiles } from './files.ts';

export interface DifferentialInput {
  readonly ts: TypeScriptApi;
  readonly tsconfig: string;
  /** The source root: each public function is imported from its own module under it. */
  readonly sourceRoot: string;
  /** The source function that makes the empty state. */
  readonly empty: string;
  readonly translation: TranslationResult;
  /** Include folders for the C++ build, in order (an override first: the double build's value types). */
  readonly includes: readonly string[];
  readonly cxx: string;
  readonly work: string;
  readonly steps: number;
  readonly seed: number;
  /** Relative tolerance of the comparison of numbers. */
  readonly tolerance: number;
  /** The target's value types the spec maps (V2 ← Vec2), with their fields (C++ name → source name). */
  readonly valueTypes: Readonly<Record<string, Readonly<Record<string, string>>>>;
}

export interface Mismatch {
  readonly step: number;
  readonly call: string;
  readonly path: string;
  readonly source: unknown;
  readonly translation: unknown;
}

export interface DifferentialResult {
  readonly steps: number;
  readonly mismatches: readonly Mismatch[];
  readonly calls: Readonly<Record<string, number>>;
}

/** The scenario calls a public function through the namespace of the module that exports it. */
function jsName(f: ApiFunction): string {
  return `m${f.module.replace(/[^A-Za-z0-9]+/g, '_')}.${f.source}`;
}

function moduleImports(t: TranslationResult, root: string): string[] {
  return [...new Set([t.stateModule, ...t.api.map((f) => f.module)])].sort()
    .map((m) => `import * as m${m.replace(/[^A-Za-z0-9]+/g, '_')} from ${JSON.stringify(pathToFileURL(path.join(root, m)).href)};`);
}

/** mulberry32: a small deterministic generator (the same scenario from the same seed). */
function generator(seed: number): () => number {
  let a = seed >>> 0;
  return () => {
    a = (a + 0x6d2b79f5) >>> 0;
    let t = a;
    t = Math.imul(t ^ (t >>> 15), t | 1);
    t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

interface Arg {
  readonly js: string;
  readonly cpp: string;
}

/** Arguments of both languages made together from a C++ type. */
class Scenario {
  readonly #rand: () => number;
  readonly #t: TranslationResult;
  readonly #values: DifferentialInput['valueTypes'];
  /** Ids issued so far (an edit that returns an integer issues one): the range ids are drawn from. */
  issued = 0;
  /** Points the scenario has made (vertices, positions): later points are drawn near them, so queries find things. */
  readonly #points: [number, number][] = [];

  constructor(seed: number, t: TranslationResult, values: DifferentialInput['valueTypes']) {
    this.#rand = generator(seed);
    this.#t = t;
    this.#values = values;
  }

  int(n: number): number {
    return Math.floor(this.#rand() * n);
  }

  pick<T>(items: readonly T[]): T {
    return items[this.int(items.length)]!;
  }

  #number(type: CppType, name: string): Arg {
    if (type.k === 'int') {
      const v = /id|track|segment|stop/i.test(name) || name === '' ? this.int(this.issued + 3) : this.int(25);
      return { js: String(v), cpp: String(v) };
    }
    // Multiples of 0.5: exact in float and double.
    const v = (this.int(1201) - 600) / 2;
    const reach = /reach|radius|dist/i.test(name);
    const r = reach ? this.int(241) / 2 : v;
    return { js: String(r), cpp: type.k === 'real' ? realLiteral(String(r), type.name) : String(r) };
  }

  value(type: CppType, name: string, state: string): Arg {
    switch (type.k) {
      case 'int':
      case 'real':
        return this.#number(type, name);
      case 'bool': {
        const b = this.#rand() < 0.5;
        return { js: String(b), cpp: String(b) };
      }
      case 'string': {
        const s = this.pick(['Alpha', 'Beta', 'Gamma', 'Delta', 'HTTP://a-b-c', '(alpha-beta)', '', 'a-']);
        return { js: JSON.stringify(s), cpp: `std::string(${JSON.stringify(s)})` };
      }
      case 'enum': {
        const e = this.#t.enums.find((x) => x.name === type.name)!;
        const v = this.pick(e.values);
        return { js: JSON.stringify(v), cpp: `${e.name}::${upper(v.replace(/[^A-Za-z0-9]+/g, '_'))}` };
      }
      case 'optional':
        if (this.#rand() < 0.4) return { js: 'undefined', cpp: 'std::nullopt' };
        return this.value(type.of, name, state);
      case 'vector': {
        if (type.of.k === 'struct' && this.#values[type.of.name]) return this.#walk(type.of.name);
        const n = 2 + this.int(3);
        const items = Array.from({ length: n }, () => this.value(type.of, name, state));
        return { js: `[${items.map((i) => i.js).join(', ')}]`, cpp: `${render(type)}{${items.map((i) => i.cpp).join(', ')}}` };
      }
      case 'struct':
        return this.#struct(type.name, name, state);
      case 'function':
        // A membership test of a number (a Set or a rule passed in as `{ has(k) }`): one fixed rule, both languages.
        if (type.ret.k === 'bool' && type.params.length === 1 && type.params[0]!.k === 'int') {
          return { js: '{ has: (k) => k % 3 !== 1 }', cpp: '[](int64_t k) { return k % 3 != 1; }' };
        }
        // One fixed predicate on a point, the same in both languages.
        return { js: '(p) => (Math.floor(p.x / 7) + Math.floor(p.y / 7)) % 2 === 0', cpp: '[](const auto &p) { return (int(std::floor(p.x / 7)) + int(std::floor(p.y / 7))) % 2 == 0; }' };
      default:
        throw new Error(`the harness cannot make a value of ${render(type)} for ${name}`);
    }
  }

  /** A polyline: a random walk, so tracks meet and cross. */
  /**
   * A point: most often near one the scenario made before (a jitter of a few metres, still a multiple of 0.5), so
   * stops lie on tracks, tracks meet at their ends and queries land where things are; else anywhere.
   */
  #point(): [number, number] {
    const near = this.#points.length > 0 && this.#rand() < 0.75;
    const jitter = (): number => (this.int(9) - 4) / 2;
    const p: [number, number] = near ? (([x, y]) => [x + jitter(), y + jitter()])(this.pick(this.#points)) as [number, number]
      : [(this.int(801) - 400) / 2, (this.int(801) - 400) / 2];
    this.#points.push(p);
    return p;
  }

  #pointArg(valueType: string, [x, y]: [number, number]): Arg {
    const fields = Object.values(this.#values[valueType]!);
    return { js: `{ ${fields[0]}: ${x}, ${fields[1]}: ${y} }`, cpp: `${valueType}(${realLiteral(String(x), 'double')}, ${realLiteral(String(y), 'double')})` };
  }

  /** A polyline: it starts near a known point and walks, so tracks meet and cross. */
  #walk(valueType: string): Arg {
    const n = 2 + this.int(5);
    let [x, y] = this.#point();
    const points: Arg[] = [];
    for (let i = 0; i < n; i++) {
      points.push(this.#pointArg(valueType, [x, y]));
      x += (this.int(121) - 60) / 2;
      y += (this.int(121) - 60) / 2;
      this.#points.push([x, y]);
    }
    return { js: `[${points.map((q) => q.js).join(', ')}]`, cpp: `std::vector<${valueType}>{${points.map((q) => q.cpp).join(', ')}}` };
  }

  #struct(name: string, hint: string, state: string): Arg {
    if (this.#values[name]) return this.#pointArg(name, this.#point());
    // A struct another public function makes from the state (a rail graph): made by that function.
    const maker = this.#t.api.find((f) => !f.edit && f.ret.k === 'struct' && f.ret.name === name);
    if (maker) {
      const args = maker.params.slice(1).map((p) => this.value(p.type, p.name, state));
      return { js: `${jsName(maker)}(state${args.map((a) => `, ${a.js}`).join('')})`, cpp: `${maker.cpp}(state${args.map((a) => `, ${a.cpp}`).join('')})` };
    }
    const struct = this.#t.structs.find((s) => s.name === name);
    if (!struct) throw new Error(`the harness knows no struct ${name} (for ${hint})`);
    const js: string[] = [];
    const positions: string[] = [];
    const cpp: string[] = [];
    const xy = struct.fields.some((f) => f.source === 'x' && f.type.k === 'real') && struct.fields.some((f) => f.source === 'y' && f.type.k === 'real')
      ? this.#point() : null;
    for (const f of struct.fields) {
      const at = xy && (f.source === 'x' || f.source === 'y') ? xy[f.source === 'x' ? 0 : 1] : null;
      const v = at !== null && f.type.k === 'real' ? { js: String(at), cpp: realLiteral(String(at), f.type.name) } : this.value(f.type, f.source, state);
      if (v.js !== 'undefined') js.push(`${f.source}: ${v.js}`);
      positions.push(v.js);
      cpp.push(v.cpp === 'std::nullopt' ? '{}' : v.cpp);
    }
    // a tuple of the source (fields by position) is an array in JavaScript
    if (isTuple(struct)) return { js: `[${positions.join(', ')}]`, cpp: `${name}{${cpp.join(', ')}}` };
    return { js: `{ ${js.join(', ')} }`, cpp: `${name}{${cpp.join(', ')}}` };
  }
}

/** A struct made from a source tuple: its fields' sources are their positions. */
function isTuple(s: CppStruct): boolean {
  return s.fields.length > 0 && s.fields.every((f, i) => f.source === String(i));
}

/** The C++ printer of every type the API returns and the state holds, normalized like the JavaScript one. */
function cppPrinter(t: TranslationResult, values: DifferentialInput['valueTypes']): string {
  const lines: string[] = [];
  const enumPut = (e: CppEnum): string => `void put(std::ostream &o, ${e.name} v) {\n\tswitch (v) {\n${e.values.map((v) =>
    `\tcase ${e.name}::${upper(v.replace(/[^A-Za-z0-9]+/g, '_'))}: o << ${JSON.stringify(JSON.stringify(v))}; return;`).join('\n')}\n\t}\n}`;
  const structPut = (s: CppStruct): string => {
    // a tuple prints as the JavaScript array it is
    if (isTuple(s)) {
      const items = s.fields.map((f, i) => `\t${i ? 'o << ","; ' : ''}put(o, v.${f.name});`);
      return `void put(std::ostream &o, const ${s.name} &v) {\n\to << "[";\n${items.join('\n')}\n\to << "]";\n}`;
    }
    const fields = [...s.fields].sort((a, b) => (a.source < b.source ? -1 : 1));
    const body = fields.map((f) => `\tif (present(v.${f.name})) { o << (first ? "" : ",") << ${JSON.stringify(JSON.stringify(f.source) + ':')}; put(o, v.${f.name}); first = false; }`);
    return `void put(std::ostream &o, const ${s.name} &v) {\n\tbool first = true;\n\to << "{";\n${body.join('\n')}\n\to << "}";\n}`;
  };
  for (const e of t.enums) lines.push(`void put(std::ostream &o, ${e.name} v);`);
  for (const s of t.structs) lines.push(`void put(std::ostream &o, const ${s.name} &v);`);
  for (const name of Object.keys(values)) lines.push(`void put(std::ostream &o, const ${name} &v);`);
  lines.push(`template <class T> void put(std::ostream &o, const std::vector<T> &v);`,
    `template <class T> void put(std::ostream &o, const std::optional<T> &v);`,
    `template <class T> void put(std::ostream &o, const T *v);`,
    `template <class T> void put(std::ostream &o, const std::shared_ptr<T> &v);`,
    `template <class K, class V> void put(std::ostream &o, const js::Map<K, V> &v);`,
    `template <class T> void put(std::ostream &o, const js::Set<T> &v);`);
  lines.push(...t.enums.map(enumPut), ...t.structs.map(structPut));
  for (const [name, fields] of Object.entries(values)) {
    const sorted = Object.entries(fields).sort((a, b) => (a[1] < b[1] ? -1 : 1));
    lines.push(`void put(std::ostream &o, const ${name} &v) {\n\to << "{";\n${sorted.map(([cpp, source], i) => `\to << "${i ? ',' : ''}\\"${source}\\":"; put(o, double(v.${cpp}));`).join('\n')}\n\to << "}";\n}`);
  }
  lines.push(
    `template <class T> void put(std::ostream &o, const std::vector<T> &v) { o << "["; for (size_t i = 0; i < v.size(); i++) { if (i) o << ","; put(o, v[i]); } o << "]"; }`,
    `template <class T> void put(std::ostream &o, const std::optional<T> &v) { if (v) put(o, *v); else o << "null"; }`,
    `template <class T> void put(std::ostream &o, const T *v) { if (v) put(o, *v); else o << "null"; }`,
    `template <class T> void put(std::ostream &o, const std::shared_ptr<T> &v) { if (v) put(o, *v); else o << "null"; }`,
    `template <class T> void put(std::ostream &o, const js::Set<T> &v) { o << "["; bool first = true; for (const auto &x : v) { if (!first) o << ","; first = false; put(o, x); } o << "]"; }`,
    `template <class K, class V> void put(std::ostream &o, const js::Map<K, V> &v) {`,
    // in insertion order, as JavaScript keeps a Map and js::Map keeps it
    `\to << "["; bool first = true; for (const auto &kv : v) { if (!first) o << ","; first = false; o << "["; put(o, kv.first); o << ","; put(o, kv.second); o << "]"; } o << "]";`,
    `}`);
  return lines.join('\n');
}

const CPP_HEAD = `#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace city {
template <class T> std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>> put(std::ostream &o, T v) { o << int64_t(v); }
inline void put(std::ostream &o, double v) {
	if (!std::isfinite(v)) { o << "null"; return; }
	char b[40];
	std::snprintf(b, sizeof b, "%.17g", v);
	o << b;
}
inline void put(std::ostream &o, float v) { put(o, double(v)); }
inline void put(std::ostream &o, bool v) { o << (v ? "true" : "false"); }
inline void put(std::ostream &o, const std::string &s) {
	o << '"';
	for (char c : s) { if (c == '"' || c == '\\\\') o << '\\\\'; o << c; }
	o << '"';
}
template <class T> bool present(const std::optional<T> &v) { return v.has_value(); }
template <class T> bool present(const T *v) { return v != nullptr; }
template <class T> bool present(const std::shared_ptr<T> &v) { return v != nullptr; }
inline bool present(bool v) { return v; }
template <class T> bool present(const T &) { return true; }
`;

const JS_SER = `const ser = (v) => {
  if (v === null || v === undefined) return null;
  if (typeof v === 'number') return Number.isFinite(v) ? (Object.is(v, -0) ? 0 : v) : null;
  if (typeof v !== 'object') return v;
  if (v instanceof Map) return [...v.entries()].map(([k, x]) => [ser(k), ser(x)]);
  if (v instanceof Set) return [...v].map(ser);
  if (Array.isArray(v) || ArrayBuffer.isView(v)) return Array.from(v, ser);
  const out = {};
  for (const k of Object.keys(v).sort()) {
    const x = ser(v[k]);
    if (x === null || x === false) continue;
    out[k] = x;
  }
  return out;
};`;

/** The resolve hook that runs the source as it is: its tsconfig path aliases, its extensionless imports. */
function hooks(input: DifferentialInput): string {
  const ts = input.ts;
  const read = ts.readConfigFile(input.tsconfig, ts.sys.readFile);
  const config = ts.parseJsonConfigFileContent(read.config, ts.sys, path.dirname(input.tsconfig));
  const base = config.options.baseUrl ?? path.dirname(input.tsconfig);
  return `import { registerHooks } from 'node:module';
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
const aliases = ${JSON.stringify(config.options.paths ?? {})};
const base = ${JSON.stringify(base)};
const file = (p) => { for (const c of [p, p + '.ts', p + '.tsx', p + '.js', path.join(p, 'index.ts')]) if (fs.existsSync(c) && fs.statSync(c).isFile()) return c; return null; };
registerHooks({
  resolve(specifier, context, next) {
    for (const [alias, targets] of Object.entries(aliases)) {
      const prefix = alias.replace(/\\*$/, '');
      if (alias.endsWith('*') ? specifier.startsWith(prefix) : specifier === alias) {
        for (const target of targets) {
          const found = file(path.resolve(base, target.replace('*', specifier.slice(prefix.length))));
          if (found) return { url: pathToFileURL(found).href, shortCircuit: true };
        }
      }
    }
    if ((specifier.startsWith('./') || specifier.startsWith('../')) && context.parentURL?.startsWith('file:')) {
      const found = file(path.resolve(path.dirname(fileURLToPath(context.parentURL)), specifier));
      if (found) return { url: pathToFileURL(found).href, shortCircuit: true };
    }
    return next(specifier, context);
  },
});
`;
}

/** Two printed values equal: numbers within the relative tolerance, everything else exactly. */
function compare(a: unknown, b: unknown, tolerance: number, where: string, out: { path: string; a: unknown; b: unknown }[]): void {
  if (typeof a === 'number' && typeof b === 'number') {
    if (Math.abs(a - b) > tolerance * Math.max(1, Math.abs(a), Math.abs(b))) out.push({ path: where, a, b });
    return;
  }
  if (Array.isArray(a) && Array.isArray(b)) {
    if (a.length !== b.length) out.push({ path: `${where}.length`, a: a.length, b: b.length });
    for (let i = 0; i < Math.min(a.length, b.length); i++) compare(a[i], b[i], tolerance, `${where}[${i}]`, out);
    return;
  }
  if (a && b && typeof a === 'object' && typeof b === 'object') {
    for (const k of new Set([...Object.keys(a), ...Object.keys(b)])) compare((a as Record<string, unknown>)[k], (b as Record<string, unknown>)[k], tolerance, `${where}.${k}`, out);
    return;
  }
  if (a !== b) out.push({ path: where, a, b });
}

export function runDifferential(input: DifferentialInput): DifferentialResult {
  const t = input.translation;
  if (t.errors.length > 0) throw new Error('the translation has errors: there is nothing to test');
  fs.mkdirSync(input.work, { recursive: true });
  const scenario = new Scenario(input.seed, t, input.valueTypes);
  const js: string[] = [];
  const cpp: string[] = [];
  const names: string[] = [];
  const calls: Record<string, number> = {};
  // Edits weigh more (they make the state the queries read); an edit that issues ids comes first.
  const weighted: ApiFunction[] = t.api.flatMap((f) => (f.edit ? (f.ret.k === 'int' ? [f, f, f, f] : [f, f]) : [f, f]));
  for (let i = 0; i < input.steps; i++) {
    const f = i < 6 ? t.api.filter((x) => x.edit && x.ret.k === 'int')[i % Math.max(1, t.api.filter((x) => x.edit && x.ret.k === 'int').length)] ?? scenario.pick(weighted) : scenario.pick(weighted);
    calls[f.source] = (calls[f.source] ?? 0) + 1;
    const args = f.params.slice(1).map((p) => scenario.value(p.type, p.name, 'state'));
    const takesState = f.params[0]?.type.k === 'struct' && (f.params[0].type as { name: string }).name === t.state;
    const allArgs = takesState ? args : [scenario.value(f.params[0]!.type, f.params[0]!.name, 'state'), ...args];
    const jsArgs = (takesState ? ['state'] : []).concat(allArgs.map((a) => a.js)).join(', ');
    const cppArgs = (takesState ? ['state'] : []).concat(allArgs.map((a) => a.cpp)).join(', ');
    names.push(`${f.source}(${allArgs.map((a) => a.js).join(', ')})`);
    if (f.edit) {
      js.push(`{ const r = ${jsName(f)}(${jsArgs}); const isData = r && typeof r === 'object' && 'data' in r && !Array.isArray(r); state = isData ? r.data : r;`
        + ` emit(${i}, isData ? Object.values(r).find((x, k) => Object.keys(r)[k] !== 'data') : null, state); }`);
      if (f.ret.k === 'void') cpp.push(`\t{ ${f.cpp}(${cppArgs}); emit(${i}, nullptr, &state); }`);
      else {
        cpp.push(`\t{ const auto r = ${f.cpp}(${cppArgs}); emit(${i}, r, &state); }`);
        scenario.issued++;
      }
    } else {
      js.push(`emit(${i}, ${jsName(f)}(${jsArgs}));`);
      cpp.push(`\temit(${i}, ${f.cpp}(${cppArgs}));`);
    }
  }
  // ---- the source, as it is
  fs.writeFileSync(path.join(input.work, 'hooks.mjs'), hooks(input));
  fs.writeFileSync(path.join(input.work, 'source.mjs'), [
    ...moduleImports(input.translation, input.sourceRoot), JS_SER,
    'const lines = [];',
    `const emit = (step, out, state) => lines.push(JSON.stringify(state === undefined ? { step, out: ser(out) } : { step, out: ser(out), state: ser(state) }));`,
    `let state = m${input.translation.stateModule.replace(/[^A-Za-z0-9]+/g, '_')}.${input.empty}();`, ...js, `process.stdout.write(lines.join('\\n') + '\\n');`].join('\n'));
  const sourceOut = execFileSync(process.execPath, ['--experimental-transform-types', '--no-warnings', '--import', pathToFileURL(path.join(input.work, 'hooks.mjs')).href,
    path.join(input.work, 'source.mjs')], { encoding: 'utf8', maxBuffer: 1 << 30 });
  // ---- the translation
  const cppDir = path.join(input.work, 'cpp');
  for (const [file, text] of t.files) {
    const out = path.join(cppDir, file);
    fs.mkdirSync(path.dirname(out), { recursive: true });
    fs.writeFileSync(out, text);
  }
  // the runtime of the built-ins, which the printer of a Map names, whether or not the translation calls it
  for (const [file, text] of runtimeFiles()) {
    fs.mkdirSync(path.dirname(path.join(cppDir, file)), { recursive: true });
    fs.writeFileSync(path.join(cppDir, file), text);
  }
  // Every module's public header: the scenario calls the feature's functions, whatever module they are in.
  const apiHeaders = [...t.files.keys()].filter((f) => f.endsWith('.h') && !f.endsWith('_types.h')).sort();
  fs.writeFileSync(path.join(input.work, 'harness.cpp'), [
    `#include "${RUNTIME_INCLUDE}"`, ...apiHeaders.map((h) => `#include "${h.replace(/^native\/src\//, '')}"`), CPP_HEAD, cppPrinter(t, input.valueTypes),
    'std::ostringstream lines;',
    // Declared before the template that calls it: names in a template are looked up where it is defined.
    `inline void put(std::ostream &o, std::nullptr_t) { o << "null"; }`,
    `template <class R> void emit(int step, const R &out, const ${t.state} *state = nullptr) {`,
    '\tlines << "{\\"out\\":"; put(lines, out);',
    '\tif (state) { lines << ",\\"state\\":"; put(lines, *state); }',
    '\tlines << ",\\"step\\":" << step << "}\\n";',
    '}',
    '} // namespace city', '', 'using namespace city;', '',
    'int main() {', `\t${t.state} state;`, ...cpp, '\tstd::cout << lines.str();', '\treturn 0;', '}', ''].join('\n'));
  const exe = path.join(input.work, 'harness.exe');
  const sources = [...t.files.keys()].filter((f) => f.endsWith('.cpp')).map((f) => path.join(cppDir, f));
  // Linked statically: the harness runs without the toolchain's runtime libraries on the path.
  execFileSync(input.cxx, ['-std=c++17', '-O2', '-static', ...input.includes.map((i) => `-I${i}`), `-I${path.join(cppDir, 'native/src')}`,
    path.join(input.work, 'harness.cpp'), ...sources, '-o', exe], { stdio: 'pipe' });
  const cppOut = execFileSync(exe, { encoding: 'utf8', maxBuffer: 1 << 30 });
  // ---- compared
  const a = sourceOut.trim().split('\n').map((l) => JSON.parse(l) as Record<string, unknown>);
  const b = cppOut.trim().split('\n').map((l) => JSON.parse(l) as Record<string, unknown>);
  const mismatches: Mismatch[] = [];
  for (let i = 0; i < Math.max(a.length, b.length); i++) {
    const found: { path: string; a: unknown; b: unknown }[] = [];
    compare(a[i], b[i], input.tolerance, '', found);
    for (const m of found) mismatches.push({ step: i, call: names[i] ?? '?', path: m.path, source: m.a, translation: m.b });
  }
  fs.writeFileSync(path.join(input.work, 'source.jsonl'), sourceOut);
  fs.writeFileSync(path.join(input.work, 'translation.jsonl'), cppOut);
  return { steps: input.steps, mismatches, calls };
}
