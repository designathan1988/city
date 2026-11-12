// The C++ runtime of the ECMAScript built-ins (cpp/js/js_runtime.h) against JavaScript itself: every case runs in
// Node and in C++ on the same inputs (edge values and seeded random ones), and the answers are compared exactly: a
// number by its 64 bits, a string by its UTF-8 bytes. A built-in the translator writes is one of these.
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { describe, it } from 'node:test';
import { cxxPath } from './support.ts';

const RUNTIME = path.join(import.meta.dirname, '..', 'cpp', 'js');

/** One built-in checked on many inputs: each input a list of argument literals, written the same in both languages. */
interface Case {
  readonly name: string;
  /** What it answers: a number (compared by bits; NaN is one value, its sign bit does not exist in JavaScript), a string (by bytes), a flag. */
  readonly kind: 'number' | 'string' | 'bool';
  /** The JavaScript expression, $0, $1 the arguments. */
  readonly js: string;
  /** The C++ expression, the same arguments. */
  readonly cpp: string;
  readonly inputs: readonly (readonly string[])[];
}

// seeded random numbers, the same each run (mulberry32)
function random(seed: number): () => number {
  return () => {
    seed = (seed + 0x6d2b79f5) | 0;
    let t = Math.imul(seed ^ (seed >>> 15), 1 | seed);
    t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}
const rand = random(20261105);
/** A double written so both languages read the same value: its 17 significant digits. */
const lit = (x: number): string => (Number.isNaN(x) ? 'NAN_' : x === Infinity ? 'INF_' : x === -Infinity ? '(-INF_)' : Object.is(x, -0) ? '(-0.0)' : x.toPrecision(17));
const str = (s: string): string => JSON.stringify(s);

const EDGE = [0, -0, 0.5, 1.5, 2.5, -1.5, -2.5, 1.005, 1.45, 1.55, 0.000001, 0.0000001, 123.456, -123.456, 1e20, 1e21, 1e-7, 5e-324,
  1.7976931348623157e308, 0.1, 0.2, 0.3, 1 / 3, 2 / 3, 4.35, 8.345, 1.0049999999999999, 2 ** 53, 2 ** 31, -(2 ** 31), 2 ** 32 + 5, NaN, Infinity, -Infinity];
const randoms = (n: number): number[] => Array.from({ length: n }, () => {
  const scale = 10 ** Math.floor(rand() * 30 - 12);
  return (rand() - 0.5) * scale;
});
const NUMBERS = [...EDGE, ...randoms(400)];
const STRINGS = ['', 'a', 'abc', '  pad  ', '\t\n x \r', 'café', '日本語テキスト', 'a😀b', 'x-y-z', 'Hello World', ' nb ', 'MiXeD', '(bracket)'];

const CASES: Case[] = [
  { name: 'Number::toString', kind: 'string', js: 'String($0)', cpp: 'js::number_to_string(double($0))', inputs: NUMBERS.map((x) => [lit(x)]) },
  { name: 'toFixed', kind: 'string', js: '($0).toFixed($1)', cpp: 'js::to_fixed($0, $1)',
    inputs: NUMBERS.filter((x) => Number.isFinite(x) || Number.isNaN(x)).flatMap((x) => [0, 1, 2, 3, 6, 20].map((f) => [lit(x), String(f)])) },
  { name: 'Math.round', kind: 'number', js: 'Math.round($0)', cpp: 'js::round($0)', inputs: NUMBERS.map((x) => [lit(x)]) },
  { name: 'Math.sign', kind: 'number', js: 'Math.sign($0)', cpp: 'js::sign($0)', inputs: NUMBERS.map((x) => [lit(x)]) },
  { name: 'Math.max', kind: 'number', js: 'Math.max($0, $1)', cpp: 'js::math_max<double>($0, $1)', inputs: EDGE.flatMap((x) => EDGE.map((y) => [lit(x), lit(y)])) },
  { name: 'Math.min', kind: 'number', js: 'Math.min($0, $1)', cpp: 'js::math_min<double>($0, $1)', inputs: EDGE.flatMap((x) => EDGE.map((y) => [lit(x), lit(y)])) },
  { name: 'Math.fround', kind: 'number', js: 'Math.fround($0)', cpp: 'js::fround($0)', inputs: NUMBERS.map((x) => [lit(x)]) },
  { name: 'ToInt32 (x | 0)', kind: 'number', js: '($0 | 0)', cpp: 'double(js::to_int32($0))', inputs: [...NUMBERS, ...randoms(100).map((x) => x * 1e12)].map((x) => [lit(x)]) },
  { name: 'Math.imul', kind: 'number', js: 'Math.imul($0, $1)', cpp: 'double(js::imul($0, $1))', inputs: NUMBERS.slice(0, 80).map((x, i) => [lit(x * 1e9), lit(NUMBERS[(i * 7) % NUMBERS.length]! * 1e9)]) },
  { name: 'Math.clz32', kind: 'number', js: 'Math.clz32($0)', cpp: 'double(js::clz32($0))', inputs: NUMBERS.map((x) => [lit(x * 1e9)]) },
  { name: 'Number.isInteger', kind: 'bool', js: 'Number.isInteger($0)', cpp: 'js::number_is_integer($0)', inputs: NUMBERS.map((x) => [lit(x)]) },
  { name: 'length', kind: 'number', js: '($0).length', cpp: 'double(js::length($0))', inputs: STRINGS.map((s) => [str(s)]) },
  { name: 'slice', kind: 'string', js: '($0).slice($1, $2)', cpp: 'js::slice($0, $1, $2)',
    inputs: STRINGS.flatMap((s) => [[0, 2], [1, -1], [-3, 10], [5, 2], [-100, 100]].map(([a, b]) => [str(s), String(a), String(b)])) },
  { name: 'substring', kind: 'string', js: '($0).substring($1, $2)', cpp: 'js::substring($0, $1, $2)',
    inputs: STRINGS.flatMap((s) => [[0, 2], [3, 1], [-3, 10], [2, 2]].map(([a, b]) => [str(s), String(a), String(b)])) },
  { name: 'indexOf', kind: 'number', js: '($0).indexOf($1)', cpp: 'double(js::index_of($0, $1))', inputs: STRINGS.flatMap((s) => ['a', 'b', '', 'é', '😀', 'z'].map((p) => [str(s), str(p)])) },
  { name: 'lastIndexOf', kind: 'number', js: '($0).lastIndexOf($1)', cpp: 'double(js::last_index_of($0, $1))', inputs: STRINGS.flatMap((s) => ['a', '-', '語'].map((p) => [str(s), str(p)])) },
  { name: 'trim', kind: 'string', js: '($0).trim()', cpp: 'js::trim($0)', inputs: STRINGS.map((s) => [str(s)]) },
  { name: 'padStart', kind: 'string', js: '($0).padStart($1, $2)', cpp: 'js::pad_start($0, $1, $2)', inputs: STRINGS.flatMap((s) => [[8, '0'], [3, 'ab'], [12, '😀']].map(([n, f]) => [str(s), String(n), str(String(f))])) },
  { name: 'padEnd', kind: 'string', js: '($0).padEnd($1, $2)', cpp: 'js::pad_end($0, $1, $2)', inputs: STRINGS.flatMap((s) => [[8, '.'], [10, 'xy']].map(([n, f]) => [str(s), String(n), str(String(f))])) },
  { name: 'charCodeAt', kind: 'number', js: '($0).charCodeAt($1)', cpp: 'js::char_code_at($0, $1)', inputs: STRINGS.flatMap((s) => [0, 1, 3, -1, 99].map((i) => [str(s), String(i)])) },
  { name: 'toLowerCase', kind: 'string', js: '($0).toLowerCase()', cpp: 'js::to_lower_case($0)', inputs: STRINGS.filter((s) => /^[\x00-\x7f]*$/.test(s)).map((s) => [str(s)]) },
  { name: 'split', kind: 'string', js: '($0).split($1).join("|")', cpp: 'js::join(js::split($0, $1), "|")', inputs: STRINGS.flatMap((s) => ['-', '', ' ', 'zz'].map((p) => [str(s), str(p)])) },
  { name: 'join of numbers', kind: 'string', js: '[$0, $1, $2].join($3)', cpp: 'js::join(std::vector<double>{$0, $1, $2}, $3)', inputs: NUMBERS.slice(0, 60).map((x, i) => [lit(x), lit(NUMBERS[i + 3]!), lit(-x / 7), str(i % 2 ? ',' : ' - ')]) },
  { name: 'sort without a comparator', kind: 'string', js: '[$0, $1, $2, $3].sort().join()', cpp: '[&] { std::vector<double> v{$0, $1, $2, $3}; js::sort(v); return js::join(v); }()',
    inputs: NUMBERS.slice(0, 50).map((x, i) => [lit(x), lit(NUMBERS[i + 5]!), lit(10), lit(9)]) },
  { name: 'includes NaN, indexOf NaN', kind: 'string', js: 'String([$0, NaN].includes(NaN)) + [$0, NaN].indexOf(NaN)', cpp: 'js::to_string(js::includes(std::vector<double>{$0, NAN_}, NAN_)) + js::to_string(js::index_of(std::vector<double>{$0, NAN_}, NAN_))', inputs: [['1'], ['0']] },
  { name: 'splice', kind: 'string', js: '(() => { const v = [1, 2, 3, 4, 5]; const r = v.splice($0, $1, 9, 8); return v.join() + "/" + r.join(); })()',
    cpp: '[&] { std::vector<double> v{1, 2, 3, 4, 5}; auto r = js::splice(v, $0, $1, 9, 8); return js::join(v) + "/" + js::join(r); }()',
    inputs: [[0, 1], [1, 2], [-2, 1], [3, 10], [9, 0], [-9, 2]].map(([a, b]) => [String(a), String(b)]) },
  { name: 'Map order', kind: 'string',
    js: '(() => { const m = new Map(); for (const k of [$0, $1, $2, $0, $3]) m.set(k, k * 2); m.delete($1); m.set($1, 1); for (const [k] of m) if (k === $2) m.delete(k); return [...m.keys()].join() + "/" + [...m.values()].join(); })()',
    cpp: '[&] { js::Map<int, int> m; for (int k : {$0, $1, $2, $0, $3}) m.insert_or_assign(k, k * 2); m.erase($1); m.insert_or_assign($1, 1); for (auto &e : m) if (e.first == $2) m.erase(e.first); std::vector<double> k, v; for (auto &e : m) { k.push_back(e.first); v.push_back(e.second); } return js::join(k) + "/" + js::join(v); }()',
    inputs: Array.from({ length: 40 }, () => [0, 1, 2, 3].map(() => String(Math.floor(rand() * 6)))) },
  { name: 'Math and Number constants', kind: 'number', js: '$0', cpp: 'js::$1', inputs: [['Math.PI', 'PI'], ['Math.E', 'E'], ['Math.LN2', 'LN2'], ['Math.LN10', 'LN10'], ['Math.LOG2E', 'LOG2E'], ['Math.LOG10E', 'LOG10E'], ['Math.SQRT2', 'SQRT2'], ['Math.SQRT1_2', 'SQRT1_2'],
    ['Number.EPSILON', 'NUMBER_EPSILON'], ['Number.MAX_VALUE', 'NUMBER_MAX_VALUE'], ['Number.MIN_VALUE', 'NUMBER_MIN_VALUE'], ['Number.MAX_SAFE_INTEGER', 'NUMBER_MAX_SAFE_INTEGER'], ['Number.MIN_SAFE_INTEGER', 'NUMBER_MIN_SAFE_INTEGER'], ['Number.POSITIVE_INFINITY', 'NUMBER_POSITIVE_INFINITY'], ['Number.NEGATIVE_INFINITY', 'NUMBER_NEGATIVE_INFINITY']] },
  { name: 'Map keys, values, from pairs, clear', kind: 'string',
    js: '(() => { const m = new Map([[$0, 1], [$1, 2], [$0, 3]]); const a = [...m.keys()].join() + "/" + [...m.values()].join(); m.clear(); return a + "/" + m.size; })()',
    cpp: '[&] { struct P { int first; int second; }; std::vector<P> pairs{{$0, 1}, {$1, 2}, {$0, 3}}; js::Map<int, int> m(pairs); std::string a = js::join(js::keys(m)) + "/" + js::join(js::values(m)); js::clear(m); return a + "/" + js::to_string(m.size()); }()',
    inputs: [['1', '2'], ['5', '5'], ['0', '9']] },
  { name: 'Set from items, spread, Array.from', kind: 'string',
    js: '(() => { const s = new Set([$0, $1, $0]); return [...s].join() + "/" + Array.from(s).length; })()',
    cpp: '[&] { js::Set<int> s(std::vector<int>{$0, $1, $0}); std::vector<int> v; v.insert(v.end(), s.begin(), s.end()); return js::join(v) + "/" + js::to_string(int(js::array_from<int>(s).size())); }()',
    inputs: [['1', '2'], ['4', '4']] },
  { name: 'Set order', kind: 'string',
    js: '(() => { const s = new Set(); for (const k of [$0, $1, $2, $0, $3]) s.add(k); s.delete($2); s.add($2); return [...s].join(); })()',
    cpp: '[&] { js::Set<int> s; for (int k : {$0, $1, $2, $0, $3}) s.insert(k); s.erase($2); s.insert($2); std::vector<double> v; for (int k : s) v.push_back(k); return js::join(v); }()',
    inputs: Array.from({ length: 30 }, () => [0, 1, 2, 3].map(() => String(Math.floor(rand() * 5)))) },
];

const fill = (template: string, args: readonly string[], js: boolean): string =>
  template.replace(/\$(\d)/g, (_, i: string) => {
    const a = args[Number(i)] ?? 'undefined';
    return js ? a.replace(/\bNAN_\b/g, 'NaN').replace(/\bINF_\b/g, 'Infinity') : a;
  });

describe('the C++ runtime of the ECMAScript built-ins', () => {
  it('answers as JavaScript does, bit for bit and byte for byte, on every case', (t) => {
    const cxx = cxxPath();
    const work = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-js-runtime-'));
    try {
      const jsLines: string[] = ['const out = [];', 'const bits = (x) => { if (Number.isNaN(x)) return "nan"; const b = new DataView(new ArrayBuffer(8)); b.setFloat64(0, x); return b.getBigUint64(0).toString(16); };',
        'const hex = (s) => Buffer.from(s, "utf8").toString("hex");'];
      const cppLines: string[] = ['#include "js_runtime.h"', '#include <cstdio>', '#include <cstring>', '#include <limits>',
        'static const double NAN_ = std::numeric_limits<double>::quiet_NaN();', 'static const double INF_ = std::numeric_limits<double>::infinity();',
        'static void number(double x) { if (x != x) { std::puts(\"nan\"); return; } uint64_t b; std::memcpy(&b, &x, 8); std::printf("%llx\\n", (unsigned long long)b); }',
        'static void text(const std::string &s) { for (unsigned char c : s) std::printf("%02x", c); std::printf("\\n"); }',
        'static void flag(bool b) { std::printf("%s\\n", b ? "true" : "false"); }', 'int main() {'];
      const labels: string[] = [];
      for (const c of CASES) {
        for (const args of c.inputs) {
          labels.push(`${c.name}(${args.join(', ')})`);
          const j = fill(c.js, args, true);
          jsLines.push(c.kind === 'number' ? `out.push(bits(${j}));` : c.kind === 'string' ? `out.push(hex(${j}));` : `out.push(String(${j}));`);
          const k = fill(c.cpp, args, false);
          cppLines.push(`\t${c.kind === 'number' ? 'number' : c.kind === 'string' ? 'text' : 'flag'}(${k});`);
        }
      }
      jsLines.push('process.stdout.write(out.join("\\n") + "\\n");');
      cppLines.push('}');
      fs.writeFileSync(path.join(work, 'cases.mjs'), jsLines.join('\n'));
      fs.writeFileSync(path.join(work, 'cases.cpp'), cppLines.join('\n'));
      const exe = path.join(work, 'cases.exe');
      execFileSync(cxx, ['-std=c++17', '-O1', '-Wall', '-Wextra', '-Werror', `-I${RUNTIME}`, path.join(work, 'cases.cpp'), '-o', exe, '-static'], { stdio: 'pipe' });
      const fromJs = execFileSync(process.execPath, [path.join(work, 'cases.mjs')], { encoding: 'utf8', maxBuffer: 1 << 28 }).trim().split('\n');
      const fromCpp = execFileSync(exe, { encoding: 'utf8', maxBuffer: 1 << 28 }).trim().split(/\r?\n/);
      assert.equal(fromCpp.length, fromJs.length);
      const wrong = labels.map((l, i) => ({ l, js: fromJs[i], cpp: fromCpp[i] })).filter((x) => x.js !== x.cpp);
      const byName = new Map<string, number>();
      const nameOf = (label: string): string => label.slice(0, label.indexOf('('));
      for (const w of wrong) byName.set(nameOf(w.l), (byName.get(nameOf(w.l)) ?? 0) + 1);
      const shown = [...byName].map(([n, k]) => `${n}: ${k}`).join(', ') + '\n' + wrong.slice(0, 20).map((w) => `${w.l}: JavaScript ${w.js}, C++ ${w.cpp}`).join('\n');
      assert.equal(wrong.length, 0, `${wrong.length} of ${labels.length} cases differ:\n${shown}`);
    } finally {
      fs.rmSync(work, { recursive: true, force: true });
    }
  });
});
