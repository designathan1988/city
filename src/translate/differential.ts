// Differential testing of a translation against its source: the same random scenario (calls of the public API with
// generated arguments) runs once in JavaScript, on the source itself (Node runs its TypeScript, its path aliases resolved
// by a module resolve hook), and once in C++, on the translation; both print one JSON line per step, normalized the same
// way (keys sorted; absent, null and false are one; maps as pairs in insertion order, as both keep it; non-finite numbers
// as null), and the lines are compared. Inputs are multiples of 0.5, exact in float and double, so a build whose numbers
// are doubles (JavaScript's own) must agree to the last bit but for the libraries' ulp; the float build is compared within
// float precision. The C++ side is one program per translation: it reads the scenario as data (which call, with which
// arguments, drawn by the scenario's seed), so another seed runs the same build. Math.random is the same seeded generator
// in both languages. Every process is a task (src/common/tasks.ts): the build is keyed by what it compiles, each run by
// what it reads. Nothing is written outside the work folder and the converter's cache.
import fs from 'node:fs';
import path from 'node:path';
import { pathToFileURL } from 'node:url';
import { filesKey, folderKey, keyedDir, keyOf, logOf, mustRun, passed, runTasksSync, textsKey, toolKey, treeKey } from '../common/tasks.ts';
import type { TypeScriptApi } from '../graph/typescript.ts';
import { render, type CppType } from './cpptype.ts';
import { enumName } from './types.ts';
import type { ApiFunction, TranslationResult } from './run.ts';
import type { CppEnum, CppStruct } from './types.ts';
import { RANDOM_CONTEXT, RUNTIME_INCLUDE } from './expressions.ts';
import { runtimeFiles } from './files.ts';

/** The source run of one scenario (Node on the TypeScript); past this its process is stopped. */
const SOURCE_TIMEOUT_S = 600;
/** The build of a translation with its harness, statically linked. */
const BUILD_TIMEOUT_S = 1800;
/** The translation's run of one scenario. */
const RUN_TIMEOUT_S = 600;
const SOURCE_RESULT = 'source.jsonl';
const TRANSLATION_RESULT = 'translation.jsonl';
const SCENARIO_DATA = 'scenario.txt';
const HARNESS_EXE = 'harness.exe';

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

/**
 * mulberry32: a small deterministic generator (the same scenario from the same seed). It is also Math.random in both
 * languages: this function's own text in the source run, MULBERRY32_CPP (the same arithmetic on uint32) in the harness.
 */
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

/** `generator` in C++: Math.random of the translation (js::Random, cpp/js/js_random.h), bit for bit the source's. */
const MULBERRY32_CPP = `struct HarnessRandom : js::Random {
	uint32_t a;
	explicit HarnessRandom(uint32_t seed) : a(seed) {}
	double next_unit() override {
		a += 0x6d2b79f5u;
		uint32_t t = a;
		t = (t ^ (t >> 15)) * (t | 1u);
		t ^= t + (t ^ (t >> 7)) * (t | 61u);
		return double(t ^ (t >> 14)) / 4294967296.0;
	}
};`;

/** The module the source run imports first: Math.random is the seeded generator before any source module loads. */
function randomModule(seed: number): string {
  return `Math.random = (${String(generator)})(${seed});\n`;
}

/** An argument of both languages: the JavaScript expression, and the tokens the harness reads it from. */
interface Arg {
  readonly js: string;
  readonly data: readonly string[];
}

/** A string as the harness reads it: its UTF-8 length, then each byte. */
function textData(s: string): string[] {
  const bytes = [...Buffer.from(s, 'utf8')];
  return [String(bytes.length), ...bytes.map(String)];
}

/** The fixed callbacks the scenario passes, the same in both languages. */
const MEMBERSHIP = { js: '{ has: (k) => k % 3 !== 1 }', cpp: '[](int64_t k) { return k % 3 != 1; }' };
const POINT_PREDICATE = { js: '(p) => (Math.floor(p.x / 7) + Math.floor(p.y / 7)) % 2 === 0', cpp: '[](const auto &p) { return (int(std::floor(p.x / 7)) + int(std::floor(p.y / 7))) % 2 == 0; }' };
const isMembership = (type: Extract<CppType, { k: 'function' }>): boolean => type.ret.k === 'bool' && type.params.length === 1 && type.params[0]!.k === 'int';
const STRINGS = ['Alpha', 'Beta', 'Gamma', 'Delta', 'HTTP://a-b-c', '(alpha-beta)', '', 'a-'];

/** The struct another public function makes from the state (a rail graph): made by that function, never field by field. */
function makerOf(t: TranslationResult, plain: boolean, name: string): ApiFunction | undefined {
  return plain ? undefined : t.api.find((f) => !f.edit && f.ret.k === 'struct' && f.ret.name === name);
}

/** Arguments of both languages made together from a C++ type: the JavaScript text and the harness's data. */
class Scenario {
  readonly #rand: () => number;
  readonly #t: TranslationResult;
  readonly #values: DifferentialInput['valueTypes'];
  /** Ids issued so far (an edit that returns an integer issues one): the range ids are drawn from. */
  issued = 0;
  /** No state to make values from: a struct is written field by field. */
  readonly plain: boolean;
  /** The call being drawn answers an integer: a fraction is no value of its domain. */
  whole = false;
  /** Points the scenario has made (vertices, positions): later points are drawn near them, so queries find things. */
  readonly #points: [number, number][] = [];

  constructor(seed: number, t: TranslationResult, values: DifferentialInput['valueTypes'], plain: boolean) {
    this.#rand = generator(seed);
    this.#t = t;
    this.#values = values;
    this.plain = plain;
  }

  int(n: number): number {
    return Math.floor(this.#rand() * n);
  }

  pick<T>(items: readonly T[]): T {
    return items[this.int(items.length)]!;
  }

  #number(type: CppType, name: string): Arg {
    // ids past what a float holds, close enough to meet: a key kept as a float makes two of them one
    if (type.k === 'int' && type.name === 'int64_t') {
      const v = 33554432 + this.int(6);
      return { js: String(v), data: [String(v)] };
    }
    if (type.k === 'int') {
      const v = /id|track|segment|stop/i.test(name) || name === '' ? this.int(this.issued + 3) : this.int(25);
      return { js: String(v), data: [String(v)] };
    }
    // Multiples of 0.5: exact in float and double; whole where the call gives an integer back (an id, a count)
    const v = this.whole ? this.int(601) - 300 : (this.int(1201) - 600) / 2;
    const reach = /reach|radius|dist/i.test(name);
    const r = reach ? this.int(241) / 2 : v;
    return { js: String(r), data: [String(r)] };
  }

  value(type: CppType, name: string, state: string): Arg {
    switch (type.k) {
      case 'int':
      case 'real':
        return this.#number(type, name);
      case 'bool': {
        const b = this.#rand() < 0.5;
        return { js: String(b), data: [b ? '1' : '0'] };
      }
      case 'string': {
        const s = this.pick(STRINGS);
        return { js: JSON.stringify(s), data: textData(s) };
      }
      case 'dynamic': {
        const n = this.int(41) - 20;
        return this.pick([{ js: String(n), data: ['0', String(n)] }, { js: '"ab"', data: ['1'] }, { js: 'null', data: ['2'] }, { js: 'true', data: ['3'] }]);
      }
      case 'set': {
        const items = Array.from({ length: this.int(4) }, () => this.value(type.of, name, state));
        return { js: `new Set([${items.map((x) => x.js).join(', ')}])`, data: [String(items.length), ...items.flatMap((x) => x.data)] };
      }
      case 'enum': {
        const e = this.#t.enums.find((x) => x.name === type.name)!;
        const v = this.pick(e.values);
        return { js: JSON.stringify(v), data: [String(e.values.indexOf(v))] };
      }
      case 'optional': {
        if (this.#rand() < 0.4) return { js: 'undefined', data: ['0'] };
        const inner = this.value(type.of, name, state);
        return { js: inner.js, data: ['1', ...inner.data] };
      }
      case 'vector': {
        if (type.of.k === 'struct' && this.#values[type.of.name]) return this.#walk(type.of.name);
        // the light check's indexes are drawn apart from its lists: long enough to hold them
        const n = this.plain ? 30 : 2 + this.int(3);
        const items = Array.from({ length: n }, () => this.value(type.of, name, state));
        return { js: `[${items.map((i) => i.js).join(', ')}]`, data: [String(n), ...items.flatMap((i) => i.data)] };
      }
      case 'struct':
        return this.#struct(type.name, name, state);
      case 'function':
        // A membership test of a number (a Set or a rule passed in as `{ has(k) }`), else one predicate on a point: fixed, both languages.
        return { js: isMembership(type) ? MEMBERSHIP.js : POINT_PREDICATE.js, data: [] };
      default:
        throw new Error(`the harness cannot make a value of ${render(type)} for ${name}`);
    }
  }

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
    return { js: `{ ${fields[0]}: ${x}, ${fields[1]}: ${y} }`, data: [String(x), String(y)] };
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
    return { js: `[${points.map((q) => q.js).join(', ')}]`, data: [String(n), ...points.flatMap((q) => q.data)] };
  }

  #struct(name: string, hint: string, state: string): Arg {
    if (this.#values[name]) return this.#pointArg(name, this.#point());
    const maker = makerOf(this.#t, this.plain, name);
    if (maker) {
      const args = maker.params.slice(1).map((p) => this.value(p.type, p.name, state));
      return { js: `${jsName(maker)}(state${args.map((a) => `, ${a.js}`).join('')})`, data: args.flatMap((a) => a.data) };
    }
    const struct = this.#t.structs.find((s) => s.name === name);
    if (!struct) throw new Error(`the harness knows no struct ${name} (for ${hint})`);
    const js: string[] = [];
    const positions: string[] = [];
    const data: string[] = [];
    const xy = struct.fields.some((f) => f.source === 'x' && f.type.k === 'real') && struct.fields.some((f) => f.source === 'y' && f.type.k === 'real')
      ? this.#point() : null;
    for (const f of struct.fields) {
      const at = xy && (f.source === 'x' || f.source === 'y') ? xy[f.source === 'x' ? 0 : 1] : null;
      const v = at !== null && f.type.k === 'real' ? { js: String(at), data: [String(at)] } : this.value(f.type, f.source, state);
      if (v.js !== 'undefined') js.push(`${f.source}: ${v.js}`);
      positions.push(v.js);
      data.push(...v.data);
    }
    // a tuple of the source (fields by position) is an array in JavaScript
    if (isTuple(struct)) return { js: `[${positions.join(', ')}]`, data };
    return { js: `{ ${js.join(', ')} }`, data };
  }
}

/**
 * How the harness reads a value of a type from the scenario's data: the C++ expression, in the same shape the scenario
 * writes it (Scenario.value decides by the same rules). Fixed by the translation alone, so one build serves every seed.
 * Composite values read their parts in statements, whose order C++ fixes.
 */
class Reader {
  readonly #t: TranslationResult;
  readonly #values: DifferentialInput['valueTypes'];
  readonly #plain: boolean;

  constructor(t: TranslationResult, values: DifferentialInput['valueTypes'], plain: boolean) {
    this.#t = t;
    this.#values = values;
    this.#plain = plain;
  }

  /** Reads into locals, in order, then makes `make(locals)`. */
  #sequence(types: readonly CppType[], names: readonly string[], make: (locals: string[]) => string): string {
    const locals = types.map((_, i) => `v${i}`);
    return `[&] { ${types.map((t, i) => `auto ${locals[i]} = ${this.read(t, names[i] ?? '')};`).join(' ')} return ${make(locals)}; }()`;
  }

  read(type: CppType, name: string): string {
    switch (type.k) {
      case 'int':
        return `${render(type)}(d.integer())`;
      case 'real':
        return `${render(type)}(d.real())`;
      case 'bool':
        return '(d.integer() != 0)';
      case 'string':
        return 'd.text()';
      case 'dynamic':
        return '[&]() -> js::Value { switch (d.integer()) { case 0: return js::Value(double(d.integer())); case 1: return js::Value("ab"); case 2: return js::Value(nullptr); default: return js::Value(true); } }()';
      case 'set':
        return `[&] { std::vector<${render(type.of)}> items; for (int64_t n = d.integer(); n > 0; n--) items.push_back(${this.read(type.of, name)}); return ${render(type)}(items); }()`;
      case 'enum': {
        const e = this.#t.enums.find((x) => x.name === type.name)!;
        return `[&] { static const ${e.name} values[] = {${e.values.map((v) => `${e.name}::${enumName(v)}`).join(', ')}}; return values[d.integer()]; }()`;
      }
      case 'optional':
        return `[&]() -> ${render(type)} { if (d.integer() == 0) return std::nullopt; return ${this.read(type.of, name)}; }()`;
      case 'vector':
        return `[&] { ${render(type)} items; for (int64_t n = d.integer(); n > 0; n--) items.push_back(${this.read(type.of, name)}); return items; }()`;
      case 'struct':
        return this.#struct(type, name);
      case 'function':
        return isMembership(type) ? MEMBERSHIP.cpp : POINT_PREDICATE.cpp;
      default:
        throw new Error(`the harness cannot make a value of ${render(type)} for ${name}`);
    }
  }

  #struct(type: Extract<CppType, { k: 'struct' }>, hint: string): string {
    const name = type.name;
    if (this.#values[name]) return `[&] { const double x = d.real(); const double y = d.real(); return ${name}(x, y); }()`;
    const maker = makerOf(this.#t, this.#plain, name);
    if (maker) {
      const params = maker.params.slice(1);
      return this.#sequence(params.map((p) => p.type), params.map((p) => p.name), (locals) => `${maker.cpp}(state${locals.map((l) => `, ${l}`).join('')})`);
    }
    const struct = this.#t.structs.find((s) => s.name === name);
    if (!struct) throw new Error(`the harness knows no struct ${name} (for ${hint})`);
    return this.#sequence(struct.fields.map((f) => f.type), struct.fields.map((f) => f.source), (locals) => `${render(type)}{${locals.join(', ')}}`);
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
    `\tcase ${e.name}::${enumName(v)}: o << ${JSON.stringify(JSON.stringify(v))}; return;`).join('\n')}\n\t}\n}`;
  const structPut = (s: CppStruct): string => {
    // a tuple prints as the JavaScript array it is
    if (isTuple(s)) {
      const items = s.fields.map((f, i) => `\t${i ? 'o << ","; ' : ''}put(o, v.${f.name});`);
      return `${putHead(s)} {\n\to << "[";\n${items.join('\n')}\n\to << "]";\n}`;
    }
    const fields = [...s.fields].sort((a, b) => (a.source < b.source ? -1 : 1));
    const body = fields.map((f) => `\tif (present(v.${f.name})) { o << (first ? "" : ",") << ${JSON.stringify(JSON.stringify(f.source) + ':')}; put(o, v.${f.name}); first = false; }`);
    return `${putHead(s)} {\n\tbool first = true;\n\to << "{";\n${body.join('\n')}\n\to << "}";\n}`;
  };
  for (const e of t.enums) lines.push(`void put(std::ostream &o, ${e.name} v);`);
  for (const s of t.structs) lines.push(`${putHead(s)};`);
  for (const name of Object.keys(values)) lines.push(`void put(std::ostream &o, const ${name} &v);`);
  lines.push(`template <class T> void put(std::ostream &o, const std::vector<T> &v);`,
    `template <class T> void put(std::ostream &o, const std::optional<T> &v);`,
    `template <class T> void put(std::ostream &o, const T *v);`,
    `template <class T> void put(std::ostream &o, const std::shared_ptr<T> &v);`,
    `void put(std::ostream &o, const js::Value &v);`,
    `template <class R, class... A> void put(std::ostream &o, const std::function<R(A...)> &) { o << "null"; }`,
    `template <class A, class B> void put(std::ostream &o, const std::pair<A, B> &v);`,
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
    `template <class A, class B> void put(std::ostream &o, const std::pair<A, B> &v) { o << "["; put(o, v.first); o << ","; put(o, v.second); o << "]"; }`,
    `void put(std::ostream &o, const js::Value &v) {
	using Kind = js::Value::Kind;
	if (v.nullish()) { o << "null"; return; }
	if (v.kind() == Kind::Boolean) { put(o, v.truthy()); return; }
	if (v.kind() == Kind::Number) { put(o, v.number()); return; }
	if (v.kind() == Kind::String) { put(o, v.text()); return; }
	if (v.is_array()) { put(o, v.items()); return; }
	std::vector<std::string> keys = v.keys();
	std::sort(keys.begin(), keys.end());
	o << "{";
	bool first = true;
	for (const std::string &k : keys) {
		const js::Value x = v.get(k);
		if (x.nullish() || (x.kind() == Kind::Boolean && !x.truthy())) continue;
		if (!first) o << ",";
		put(o, k);
		o << ":";
		put(o, x);
		first = false;
	}
	o << "}";
}`,
    `template <class T> void put(std::ostream &o, const js::Set<T> &v) { o << "["; bool first = true; for (const auto &x : v) { if (!first) o << ","; first = false; put(o, x); } o << "]"; }`,
    `template <class K, class V> void put(std::ostream &o, const js::Map<K, V> &v) {`,
    // in insertion order, as JavaScript keeps a Map and js::Map keeps it
    `\to << "["; bool first = true; for (const auto &kv : v) { if (!first) o << ","; first = false; o << "["; put(o, kv.first); o << ","; put(o, kv.second); o << "]"; } o << "]";`,
    `}`);
  return lines.join('\n');
}

/** How a struct is printed; of a generic class, for any of its instances. */
function putHead(s: { readonly name: string; readonly typeParams?: readonly string[] }): string {
  const params = s.typeParams ?? [];
  if (params.length === 0) return `void put(std::ostream &o, const ${s.name} &v)`;
  return `template <${params.map((p) => `class ${p}`).join(', ')}> void put(std::ostream &o, const ${s.name}<${params.join(', ')}> &v)`;
}

const CPP_HEAD = `#include <algorithm>
#include <functional>
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
	for (char c : s) {
		if (static_cast<unsigned char>(c) < 0x20) { char code[8]; std::snprintf(code, sizeof code, "\\\\u%04x", unsigned(c)); o << code; continue; }
		if (c == '"' || c == '\\\\') o << '\\\\';
		o << c;
	}
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
  if (typeof v === 'function') return null;
  if (typeof v !== 'object') return v;
  if (v instanceof Map) return [...v.entries()].map(([k, x]) => [ser(k), ser(x)]);
  if (v instanceof Set) return [...v].map(ser);
  if (typeof v[Symbol.iterator] === 'function') return Array.from(v, ser);
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
  if ((a === null || a === undefined) && (b === null || b === undefined)) return;
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

/** What the harness reads the scenario with: whitespace-separated tokens (integers, reals as JavaScript prints them). */
const DATA_READER = `struct ScenarioData {
	std::istream &in;
	int64_t integer() { long long v = 0; in >> v; return int64_t(v); }
	double real() { std::string t; in >> t; return std::strtod(t.c_str(), nullptr); }
	std::string text() { std::string s; for (int64_t n = integer(); n > 0; n--) s.push_back(char(integer())); return s; }
};`;

/** The harness's own headers beyond the printer's. */
const HARNESS_INCLUDES = ['#include <cstdint>', '#include <cstdlib>', '#include <fstream>'];

/** A call the harness can make: how its arguments are read and what it does with them. */
interface HarnessCall {
  /** Each argument read from the data, in order. */
  readonly params: readonly { readonly name: string; readonly type: CppType }[];
  /** The statement that calls it with the locals read (and the context) and emits its answer for `step`. */
  readonly run: (args: readonly string[]) => string;
}

/** One `case` per call: its arguments read into locals in order, then the call. */
function dispatch(reader: Reader, calls: readonly HarnessCall[]): string[] {
  return calls.map((c, k) => {
    const locals = c.params.map((p, i) => `\t\t\tauto a${i} = ${reader.read(p.type, p.name)};`);
    return [`\t\tcase ${k}: {`, ...locals, `\t\t\t${c.run(c.params.map((_, i) => `a${i}`))}`, '\t\t\tbreak;', '\t\t}'].join('\n');
  });
}

/** The harness's main: the generator seeded from the data's first token, what `prelude` makes, then each step's call. */
function harnessMain(prelude: readonly string[], cases: readonly string[]): string[] {
  return ['int main(int argc, char **argv) {', '\tif (argc != 3) return 2;', '\tstd::ifstream file(argv[1]);', '\tScenarioData d{file};',
    `\tHarnessRandom random_source(uint32_t(d.integer()));`, `\tjs::Random &${RANDOM_CONTEXT.name} = random_source;`, `\t(void)${RANDOM_CONTEXT.name};`,
    ...prelude, '\tint64_t step = 0;', '\twhile (file >> step) {', '\t\tswitch (d.integer()) {', ...cases, '\t\tdefault: return 3;', '\t\t}', '\t}',
    '\tstd::ofstream out(argv[2], std::ios::binary);', '\tout << lines.str();', '\treturn out ? 0 : 4;', '}', ''];
}

interface Build {
  readonly exe: string;
  readonly key: string;
}

/**
 * The translation and its harness compiled once, in the cache folder its content names (the files, the compiler, the
 * flags, the include folders' content): every seed, every run and every work folder with the same content use it.
 */
function build(files: ReadonlyMap<string, string>, harness: string, cxx: string, flags: readonly string[], includes: readonly string[], ownFirst: boolean, what: string): Build {
  const all = new Map<string, string>([...[...files].map(([f, t]) => [`cpp/${f}`, t] as [string, string]), ['harness.cpp', harness]]);
  const key = keyOf('differential-build', textsKey(all), cxx, toolKey(cxx), ...flags, ...includes.map((i) => `${i}\0${folderKey(i)}`), String(ownFirst));
  const dir = keyedDir('differential', key);
  if (!fs.existsSync(path.join(dir, 'harness.cpp'))) {
    for (const [file, text] of all) {
      fs.mkdirSync(path.dirname(path.join(dir, file)), { recursive: true });
      fs.writeFileSync(path.join(dir, file), text);
    }
  }
  const exe = path.join(dir, HARNESS_EXE);
  const own = '-Icpp/native/src';
  const others = includes.map((i) => `-I${i.split(path.sep).join('/')}`);
  const sources = [...files.keys()].filter((f) => f.endsWith('.cpp')).map((f) => `cpp/${f}`);
  // Linked statically: the harness runs without the toolchain's runtime libraries on the path.
  const [r] = runTasksSync([{ name: 'converter-differential-build', cwd: dir, extra: [key], outputs: [exe], timeoutS: BUILD_TIMEOUT_S,
    argv: [cxx, ...flags, ...(ownFirst ? [own, ...others] : [...others, own]), 'harness.cpp', ...sources, '-o', HARNESS_EXE] }]);
  if (!passed(mustRun(`the build of ${what}`, r!))) {
    throw new Error(`${what} does not compile: ${logOf(r!).split('\n').filter((l) => l.includes('error:')).slice(0, 6).join('\n')}`);
  }
  return { exe, key };
}

/** The translation's run of one scenario: the build reading the data, its answers in the work folder. */
function runTranslation(b: Build, work: string, data: string): string {
  const dataFile = path.join(work, SCENARIO_DATA);
  const out = path.join(work, TRANSLATION_RESULT);
  fs.writeFileSync(dataFile, data);
  const [r] = runTasksSync([{ name: 'converter-differential-run', cwd: work, extra: [b.key, textsKey([['data', data]])], outputs: [out], timeoutS: RUN_TIMEOUT_S,
    argv: [b.exe, SCENARIO_DATA, TRANSLATION_RESULT] }]);
  if (!passed(mustRun('the translation\'s run', r!)) || !fs.existsSync(out)) throw new Error(`the translation's run failed (exit ${r!.exit}):\n${logOf(r!).slice(-4000)}`);
  return fs.readFileSync(out, 'utf8');
}

/** The source's run of one scenario: Node on the TypeScript, Math.random seeded first, its answers in the work folder. */
function runSource(input: { readonly sourceRoot: string; readonly work: string; readonly seed: number; readonly hooksFile?: string | undefined }, source: string, hooksText: string | null): string {
  const random = path.join(input.work, 'random.mjs');
  const hooks = input.hooksFile ?? path.join(input.work, 'hooks.mjs');
  const script = path.join(input.work, 'source.mjs');
  const out = path.join(input.work, SOURCE_RESULT);
  fs.writeFileSync(random, randomModule(input.seed));
  if (hooksText !== null) fs.writeFileSync(hooks, hooksText);
  fs.writeFileSync(script, source);
  const [r] = runTasksSync([{ name: 'converter-differential-source', cwd: input.work, timeoutS: SOURCE_TIMEOUT_S, outputs: [out],
    extra: [treeKey(input.sourceRoot), filesKey([random, hooks, script]), process.version],
    argv: [process.execPath, '--experimental-transform-types', '--no-warnings', '--import', pathToFileURL(random).href, '--import', pathToFileURL(hooks).href, script, SOURCE_RESULT] }]);
  if (!passed(mustRun('the source\'s run', r!)) || !fs.existsSync(out)) throw new Error(`the source failed (exit ${r!.exit}):\n${logOf(r!).slice(-4000)}`);
  return fs.readFileSync(out, 'utf8');
}

function linesOf(text: string): Record<string, unknown>[] {
  return text.trim().split('\n').filter(Boolean).map((l) => JSON.parse(l) as Record<string, unknown>);
}

export function runDifferential(input: DifferentialInput): DifferentialResult {
  const t = input.translation;
  if (t.errors.length > 0) throw new Error('the translation has errors: there is nothing to test');
  fs.mkdirSync(input.work, { recursive: true });
  const scenario = new Scenario(input.seed, t, input.valueTypes, false);
  const js: string[] = [];
  const data: string[] = [String(input.seed >>> 0)];
  const names: string[] = [];
  const calls: Record<string, number> = {};
  const takesState = (f: ApiFunction): boolean => f.params[0]?.type.k === 'struct' && (f.params[0].type as { name: string }).name === t.state;
  const withContext = (args: readonly string[], f: ApiFunction): string => [...args, ...(f.context ?? []).map((c) => c.name)].join(', ');
  // Edits weigh more (they make the state the queries read); an edit that issues ids comes first.
  const weighted: ApiFunction[] = t.api.flatMap((f) => (f.edit ? (f.ret.k === 'int' ? [f, f, f, f] : [f, f]) : [f, f]));
  for (let i = 0; i < input.steps; i++) {
    const f = i < 6 ? t.api.filter((x) => x.edit && x.ret.k === 'int')[i % Math.max(1, t.api.filter((x) => x.edit && x.ret.k === 'int').length)] ?? scenario.pick(weighted) : scenario.pick(weighted);
    calls[f.source] = (calls[f.source] ?? 0) + 1;
    const args = f.params.slice(1).map((p) => scenario.value(p.type, p.name, 'state'));
    const allArgs = takesState(f) ? args : [scenario.value(f.params[0]!.type, f.params[0]!.name, 'state'), ...args];
    const jsArgs = (takesState(f) ? ['state'] : []).concat(allArgs.map((a) => a.js)).join(', ');
    names.push(`${f.source}(${allArgs.map((a) => a.js).join(', ')})`);
    data.push([String(i), String(t.api.indexOf(f)), ...allArgs.flatMap((a) => a.data)].join(' '));
    if (f.edit) {
      js.push(`{ const r = ${jsName(f)}(${jsArgs}); const isData = r && typeof r === 'object' && 'data' in r && !Array.isArray(r); state = isData ? r.data : r;`
        + ` emit(${i}, isData ? Object.values(r).find((x, k) => Object.keys(r)[k] !== 'data') : null, state); }`);
      if (f.ret.k !== 'void') scenario.issued++;
    } else {
      js.push(`emit(${i}, ${jsName(f)}(${jsArgs}));`);
    }
  }
  // ---- the source, as it is
  const sourceOut = runSource(input, [
    "import { writeFileSync } from 'node:fs';", ...moduleImports(input.translation, input.sourceRoot), JS_SER,
    'const lines = [];',
    `const emit = (step, out, state) => lines.push(JSON.stringify(state === undefined ? { step, out: ser(out) } : { step, out: ser(out), state: ser(state) }));`,
    `let state = m${input.translation.stateModule.replace(/[^A-Za-z0-9]+/g, '_')}.${input.empty}();`, ...js, `writeFileSync(process.argv[2], lines.join('\\n') + '\\n');`].join('\n'), hooks(input));
  // ---- the translation: one build for every scenario, reading this one's calls as data
  const reader = new Reader(t, input.valueTypes, false);
  const harnessCalls: HarnessCall[] = t.api.map((f) => ({
    params: takesState(f) ? f.params.slice(1) : f.params,
    run: (locals) => {
      const args = withContext((takesState(f) ? ['state'] : []).concat(locals), f);
      if (!f.edit) return `emit(step, ${f.cpp}(${args}));`;
      return f.ret.k === 'void' ? `{ ${f.cpp}(${args}); emit(step, nullptr, &state); }` : `{ const auto r = ${f.cpp}(${args}); emit(step, r, &state); }`;
    },
  }));
  // Every module's public header: the scenario calls the feature's functions, whatever module they are in.
  const apiHeaders = [...t.files.keys()].filter((f) => f.endsWith('.h') && !f.endsWith('_types.h')).sort();
  const harness = [
    `#include "${RUNTIME_INCLUDE}"`, ...apiHeaders.map((h) => `#include "${h.replace(/^native\/src\//, '')}"`), ...HARNESS_INCLUDES, CPP_HEAD, cppPrinter(t, input.valueTypes),
    'std::ostringstream lines;',
    // Declared before the template that calls it: names in a template are looked up where it is defined.
    `inline void put(std::ostream &o, std::nullptr_t) { o << "null"; }`,
    `template <class R> void emit(int64_t step, const R &out, const ${t.state} *state = nullptr) {`,
    '\tlines << "{\\"out\\":"; put(lines, out);',
    '\tif (state) { lines << ",\\"state\\":"; put(lines, *state); }',
    '\tlines << ",\\"step\\":" << step << "}\\n";',
    '}', DATA_READER, MULBERRY32_CPP,
    '} // namespace city', '', 'using namespace city;', '',
    ...harnessMain([`\t${t.state} state;`], dispatch(reader, harnessCalls))].join('\n');
  // the runtime of the built-ins, which the printer of a Map names, whether or not the translation calls it
  const files = new Map([...t.files, ...runtimeFiles()]);
  const cppOut = runTranslation(build(files, harness, input.cxx, ['-std=c++17', '-O2', '-static'], input.includes, false, 'the translation'), input.work, data.join('\n') + '\n');
  // ---- compared
  const a = linesOf(sourceOut);
  const b = linesOf(cppOut);
  const mismatches: Mismatch[] = [];
  for (let i = 0; i < Math.max(a.length, b.length); i++) {
    const found: { path: string; a: unknown; b: unknown }[] = [];
    compare(a[i], b[i], input.tolerance, '', found);
    for (const m of found) mismatches.push({ step: i, call: names[i] ?? '?', path: m.path, source: m.a, translation: m.b });
  }
  return { steps: input.steps, mismatches, calls };
}

/** What the light check needs beyond the comparison: the numbers the game's context holds (`tn.<name>`). */
export interface ObjectDifferentialInput extends Omit<DifferentialInput, 'empty'> {
  readonly context?: Readonly<Record<string, number>>;
  /** A module to --import that resolves the source as its bundler does; default: the paths of the tsconfig. */
  readonly hooksFile?: string;
}

/** A value that cannot be written as an argument: an instance, a callback. */
const notAnArgument = (t: CppType): boolean => t.k === 'ref' || t.k === 'function' || t.k === 'lambda' || t.k === 'ptr' || t.k === 'tparam'
  || ((t.k === 'vector' || t.k === 'set' || t.k === 'optional') && notAnArgument(t.of));

/**
 * The classes of a translation against the source, with no game: one instance of each, its members called in a random order with
 * random arguments, each answer compared. Only the translated files are compiled, so an area is checked in seconds and without a
 * script written by hand; the free functions it exports are drawn among them.
 */
export function runObjectDifferential(input: ObjectDifferentialInput): DifferentialResult {
  const t = input.translation;
  if (t.errors.length > 0) throw new Error('the translation has errors: there is nothing to test');
  fs.mkdirSync(input.work, { recursive: true });
  // the target's value types this translation names: the others are in headers it does not include
  const texts = [...t.files.values()];
  const values = Object.fromEntries(Object.entries(input.valueTypes).filter(([name]) => texts.some((text) => text.includes(name))));
  const scenario = new Scenario(input.seed, t, values, true);
  const reader = new Reader(t, values, true);
  const ns = (module: string): string => `m${module.replace(/[^A-Za-z0-9]+/g, '_')}`;
  const withContext = (args: readonly string[], context?: readonly { readonly name: string }[]): string => [...args, ...(context ?? []).map((c) => c.name)].join(', ');
  const js: string[] = [];
  const made: string[] = [];
  const data: string[] = [String(input.seed >>> 0)];
  const calls: Record<string, number> = {};
  const names = new Map<number, string>();
  const steps: { readonly step: number; readonly line: string }[] = [];
  type Call = HarnessCall & { name: string; context?: ApiFunction['context']; ret: CppType; on: (args: readonly string[]) => string };
  const callable: Call[] = [];
  t.classes.forEach((c, i) => {
    if (c.made.params.some((p) => notAnArgument(p.type))) return;
    const args = c.made.params.map((p) => scenario.value(p.type, p.name, 'state'));
    js.push(`const o${i} = new ${ns(c.module)}.${c.source}(${args.map((a) => a.js).join(', ')});`);
    data.push(args.flatMap((a) => a.data).join(' '));
    const params = c.made.params;
    made.push(`\tauto o${i} = [&] { ${params.map((p, j) => `auto c${j} = ${reader.read(p.type, p.name)};`).join(' ')} return std::make_shared<${c.struct}>(${withContext(params.map((_, j) => `c${j}`), c.made.context)}); }();`);
    for (const m of c.members) {
      if (m.params.some((p) => notAnArgument(p.type) || p.mutable)) continue;
      const silent = m.ret.k === 'void' || m.ret.k === 'ref';
      callable.push({ name: `${c.source}.${m.source}`, params: m.params, ...(m.context ? { context: m.context } : {}), ret: m.ret,
        on: (a) => (m.getter ? `o${i}.${m.source}` : `o${i}.${m.source}(${a.join(', ')})`),
        run: (locals) => (silent ? `o${i}->${m.cpp}(${withContext(locals, m.context)}); emit(step, nullptr);` : `emit(step, o${i}->${m.cpp}(${withContext(locals, m.context)}));`) });
    }
  });
  for (const f of t.api) {
    // a state is made by its edits, with its indexes: never written as a value
    const takesState = f.params.some((p) => p.type.k === 'struct' && p.type.name === t.state);
    if (f.edit || takesState || f.params.some((p) => notAnArgument(p.type)) || f.ret.k === 'ref') continue;
    const silent = f.ret.k === 'void';
    callable.push({ name: f.source, params: f.params, ...(f.context ? { context: f.context } : {}), ret: f.ret, on: (a) => `${jsName(f)}(${a.join(', ')})`,
      run: (locals) => (silent ? `${f.cpp}(${withContext(locals, f.context)}); emit(step, nullptr);` : `emit(step, ${f.cpp}(${withContext(locals, f.context)}));`) });
  }
  if (callable.length === 0) throw new Error('nothing of this translation can be called');
  for (let i = 0; i < input.steps; i++) {
    const c = scenario.pick(callable);
    let args: Arg[];
    scenario.whole = c.ret.k === 'int';
    try {
      args = c.params.map((p) => scenario.value(p.type, p.name, 'state'));
    } catch {
      continue; // a value the scenario cannot make: this call is not drawn
    }
    calls[c.name] = (calls[c.name] ?? 0) + 1;
    names.set(i, `${c.name}(${args.map((a) => a.js).join(', ')})`);
    // what returns an instance is called for what it does: the answers that follow show it
    const silent = c.ret.k === 'void' || c.ret.k === 'ref';
    const source = c.on(args.map((a) => a.js));
    // arguments drawn at random may be refused: what the source throws on is no call of the program, and the translation is not asked
    js.push(`try { ${silent ? `${source}; emit(${i}, null);` : `emit(${i}, ${source});`} } catch { thrown.push(${i}); }`);
    steps.push({ step: i, line: [String(i), String(callable.indexOf(c)), ...args.flatMap((a) => a.data)].join(' ') });
  }
  const modules = [...new Set([...t.classes.map((c) => c.module), ...t.api.map((f) => f.module)])].sort();
  const sourceOut = runSource(input, [
    "import { writeFileSync } from 'node:fs';",
    ...modules.map((m) => `import * as ${ns(m)} from ${JSON.stringify(pathToFileURL(path.join(input.sourceRoot, m)).href)};`), JS_SER,
    'const lines = [];', 'const thrown = [];', 'const emit = (step, out) => lines.push(JSON.stringify({ step, out: ser(out) }));', ...js,
    `writeFileSync(process.argv[2], [...lines, JSON.stringify({ thrown })].join('\\n') + '\\n');`].join('\n'), input.hooksFile ? null : hooks({ ...input, empty: '' }));
  const a = linesOf(sourceOut);
  const refused = new Set((a.pop() as { thrown: number[] }).thrown);
  if (a.length === 0) throw new Error('nothing of this translation can be called');
  data.push(...steps.filter((s) => !refused.has(s.step)).map((s) => s.line));
  const apiHeaders = [...t.files.keys()].filter((f) => f.endsWith('.h') && !f.endsWith('_types.h') && !f.endsWith('_enums.h')).sort();
  const harness = [
    `#include "${RUNTIME_INCLUDE}"`, ...(input.context ? ['#include "sim/catalog/catalog.h"'] : []), ...apiHeaders.map((h) => `#include "${h.replace(/^native\/src\//, '')}"`),
    ...HARNESS_INCLUDES, CPP_HEAD, cppPrinter(t, values),
    'std::ostringstream lines;', 'inline void put(std::ostream &o, std::nullptr_t) { o << "null"; }',
    'template <class R> void emit(int64_t step, const R &out) { lines << "{\\"out\\":"; put(lines, out); lines << ",\\"step\\":" << step << "}\\n"; }',
    DATA_READER, MULBERRY32_CPP, '} // namespace city', '', 'using namespace city;', '',
    ...harnessMain([...(input.context ? ['\tTuning tn;', ...Object.entries(input.context).map(([k, v]) => `\ttn.${k} = ${v};`)] : []), ...made], dispatch(reader, callable))].join('\n');
  // the translation's own headers first: the game may hold an older copy of the runtime
  const files = new Map([...t.files, ...runtimeFiles()]);
  const cppOut = runTranslation(build(files, harness, input.cxx, ['-std=c++17', '-O0', '-static'], input.includes, true, 'the check'), input.work, data.join('\n') + '\n');
  const b = linesOf(cppOut);
  const mismatches: Mismatch[] = [];
  for (let i = 0; i < Math.max(a.length, b.length); i++) {
    const found: { path: string; a: unknown; b: unknown }[] = [];
    compare(a[i], b[i], input.tolerance, '', found);
    for (const m of found) mismatches.push({ step: i, call: names.get(a[i]?.['step'] as number) ?? '?', path: m.path, source: m.a, translation: m.b });
  }
  return { steps: input.steps, mismatches, calls };
}
