// Tests of the translation on a fixture feature whose every function is a case the flow of values must decide:
// integers through cycles, a value set inside a callback, an annotation narrower than what is returned, containers
// filled through an alias, a 64-bit key, references to elements and to an index, JavaScript's rounding, and an edit
// that reads an index (refused). The C++ written must compile without a warning against the target's headers.
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { after, describe, it } from 'node:test';
import { removeTree } from '../src/common/files.ts';
import { runDifferential, runObjectDifferential } from '../src/translate/differential.ts';
import { translate } from '../src/translate/run.ts';
import { loadTypeScript } from '../src/graph/typescript.ts';
import { readTranslationSpec } from '../src/translate/spec.ts';
import { cxxPath, typescriptDir } from './support.ts';

const root = path.join(import.meta.dirname, 'fixtures', 'translate');
const target = path.join(root, 'target');
const spec = readTranslationSpec(path.join(root, 'yard.translate.json'));
const input = { sourceRoot: root, tsconfig: path.join(root, 'tsconfig.json'), typescriptDir: typescriptDir(), targetRoot: target,
  targetGraph: path.join(target, 'cppgraph.json') };

describe('translate', () => {
  const result = translate({ ...input, spec });
  const file = (name: string): string => result.files.get(`native/src/sim/yard/${name}`) ?? '';
  const all = [...result.files.values()].join('\n');

  it('translates the whole feature', () => {
    assert.deepEqual(result.errors, []);
    assert.deepEqual([...result.files.keys()].sort(), [
      ...['geo.h', 'geo_query.cpp', 'geo_types.h', 'js_array.h', 'js_map.h', 'js_number.h', 'js_object.h', 'js_random.h', 'js_runtime.h', 'js_string.h', 'js_value.h'].map((f) => `native/src/core/${f}`),
      ...['yard.h', 'yard_edit.cpp', 'yard_enums.h', 'yard_index.cpp', 'yard_query.cpp', 'yard_types.h'].map((f) => `native/src/sim/yard/${f}`),
    ]);
  });

  it('names a literal the target forbids bare from what receives it, in its module', () => {
    assert.match(file('yard_types.h'), /constexpr int MAX_SPEED = 30;/);
    assert.match(file('yard_query.cpp'), /const float speed = js::math_min\(float\(MAX_SPEED\), n\);/);
  });

  it('finds the target types by shape, within what the layers may include', () => {
    assert.deepEqual(result.valueTypes, { PostLookC: { height: 'height', lit: 'lit' }, V2: { x: 'x', y: 'y' } }, 'Vec2 is the core V2; the same-shaped render Point2 is not includable; PostLook is the one the spec declares');
    assert.match(file('yard_types.h'), /#include "core\/math\.h"/);
  });

  it('translates shared code once, into its own owner, and the feature includes it', () => {
    assert.match(result.files.get('native/src/core/geo.h')!, /float dist\(const V2 &a, const V2 &b\);/);
    assert.match(file('yard_query.cpp'), /#include "core\/geo\.h"/);
    assert.match(file('yard_query.cpp'), /if \(dist\(V2\(q->x, q->y\), p\) < reach\)/);
    assert.doesNotMatch(file('yard_query.cpp'), /float dist\(/, 'the helper is not copied into the feature');
  });

  it('copies a spread before sorting it, and reads a tuple by position', () => {
    assert.match(file('yard_query.cpp'), /std::vector<float> sorted_\d+ = std::vector<float>\(values\);\n\tstd::stable_sort\(sorted_\d+\.begin\(\)/,
      'the caller\'s values stay as they are');
    assert.match(file('yard_types.h'), /struct NumberPair \{\n\tfloat first = 0;\n\tfloat second = 0;\n\};/);
    assert.match(file('yard_query.cpp'), /float t = item_\d+\.first;\n\t\tfloat y = item_\d+\.second;/);
  });

  it('takes a membership test as a predicate: a Set passed in, a rule written in place', () => {
    assert.match(file('yard_query.cpp'), /int kept_count\(const YardState &t, const std::function<bool\(int64_t\)> &kept\)/);
    assert.match(file('yard_query.cpp'), /if \(kept\(p\.id\)\)/);
    assert.match(file('yard_query.cpp'), /kept_count\(t, \[&\]\(int64_t k\) \{ return chosen\.count\(k\) != 0; \}\)/, 'a Set asks its hash set');
    assert.match(file('yard_query.cpp'), /std::function<bool\(int64_t\)> even = \[&\]\(int64_t id\) -> bool \{\n\t\treturn id % 2 == 0 \|\| \(chosen\.count\(int\(id\)\) != 0\);/);
  });

  it('folds with reduce, tests a prefix and a suffix, writes NaN and a number without its separator', () => {
    const q = file('yard_query.cpp');
    assert.match(q, /s_\d+\.compare\(0, p_\d+\.size\(\), p_\d+\) == 0/);
    assert.match(q, /return std::numeric_limits<float>::quiet_NaN\(\);/);
    assert.match(q, /float acc_\d+ = 0; for \(float b : /);
    assert.match(file('yard_types.h'), /constexpr float SUMMED = 1000\.5f;/);
    assert.match(file('yard_types.h'), /#include <limits>/, 'a header from the names written');
  });

  it('keeps integers integers, through cycles, callbacks and helpers called with integers', () => {
    assert.match(file('yard_query.cpp'), /\bint n = 0;/);
    assert.match(file('yard_types.h'), /struct Nearest \{\n\tint32_t i = 0;\n\tfloat d = 0;\n\tV2 at;\n\};/);
    assert.match(file('yard_query.cpp'), /js::Map<int64_t, std::vector<int>> at;/);
    assert.match(file('yard_query.cpp'), /auto link = \[&\]\(int32_t a, int32_t b\) -> void/);
    assert.match(file('yard_types.h'), /int32_t next_id = 1;/, 'the empty state starts ids at 1');
  });

  it('follows values through a class: what a method returns, a getter, a default parameter value', () => {
    assert.match(file('yard.h'), /const Post \* post_by_id\(const YardState &t, int32_t id\);/, 'the id a method returns is an integer');
  });

  it('gives a 64-bit key its width, from the target owner of keys', () => {
    assert.match(file('yard_index.cpp'), /const int64_t key = cell_key\(/);
    assert.doesNotMatch(all, /32768|65536/);
  });

  it('types a return by what is returned (TypeScript is structural; narrowing misses callbacks)', () => {
    assert.match(file('yard.h'), /std::optional<Nearest> nearest\(const YardState &t, const V2 &p\);/);
  });

  it('shares what the source shares: references, and an index that holds pointers into the state', () => {
    assert.match(file('yard_query.cpp'), /const auto &cells = t\.index\.post_cells\.cells;/);
    assert.match(file('yard_query.cpp'), /const auto &first = t\.posts\[0\];/);
    assert.match(file('yard_types.h'), /js::Map<int64_t, std::vector<const Post \*>> cells;/);
    assert.match(file('yard_index.cpp'), /index\.cells\[key\]\.push_back\(&p\);/, 'one lookup makes the entry when absent');
  });

  it('rounds the JavaScript way and edits in place', () => {
    assert.match(file('yard_query.cpp'), /std::floor\(x_\d+\); return int64_t\(x_\d+ - down >= 0\.5f \? down \+ 1 : down\)/);
    assert.match(file('yard_edit.cpp'), /t\.posts\.push_back\(Post\{id, x, y, \{\}\}\);/);
    assert.match(file('yard_edit.cpp'), /t\.posts\.erase\(std::remove_if\(t\.posts\.begin\(\), t\.posts\.end\(\), \[&\]\(const Post &p\) \{ return p\.id == id; \}\), t\.posts\.end\(\)\);/);
  });

  it('refuses an edit that reads an index of the state it changes', () => {
    const bad = translate({ ...input, spec: { ...spec, source: 'src/world/bad.ts', units: ['addItem'], state: { ...spec.state, source: 'BinData', target: 'BinState', empty: undefined } } });
    assert.ok(bad.errors.some((e) => e.includes('the edit addItem reads an index')), bad.errors.join('\n'));
  });

  it('leaves out of a class the member with no C++ form and what calls it, and keeps the class', () => {
    assert.equal(result.dropped.length, 2, result.dropped.join('; '));
    assert.match(file('yard_types.h'), /\bdoubled\(\);/);
    assert.doesNotMatch(file('yard_types.h'), /\bnamed\(\);/);
  });

  it('writes enumerations and texts as the lab needs them: a member beside a constant of its name, a made text, an optional member', () => {
    // three causes of the whole lab's header errors (2026-11-08, log/conversor.md 22:55): `FacadePropKind::POT()` (a name
    // rewritten in the constants' text), an enum made of a shader's text (TypeScript's literal type of a const is its value,
    // not an enumeration) and a literal in an optional field of an enumeration (`style?: BodyStyle`, `style: 'hatch'`)
    assert.match(all, /std::vector<PropKind>\{PropKind::POT, PropKind::BUSH\}/);
    assert.doesNotMatch(all, /PropKind::POT\(\)/);
    assert.match(file('yard_query.cpp'), /\bPOT\(\)/, 'the constant made by a call is called');
    assert.doesNotMatch(all, /enum class \w+ : int \{ ATTRIBUTE/);
    assert.match(all, /const std::string &PARS\(\)|const std::string PARS\b/);
    assert.match(all, /std::vector<Prop> PROPS = std::vector<Prop>\{Prop\{PropKind::BUSH\}, Prop\{\{\}\}\};/, 'the literal is the member, the left-out field is absent');
  });

  it('makes an interface classes implement an abstract struct, a value of it read through an accessor', () => {
    const types = file('yard_types.h');
    assert.match(types, /struct Gauge \{\n\tvirtual ~Gauge\(\) = default;/, types);
    assert.match(types, /virtual float get_level\(\) = 0;/);
    assert.match(types, /virtual float read\(float scale\) = 0;/);
    assert.match(types, /struct Tank : std::enable_shared_from_this<Tank>, Gauge \{/);
    assert.match(types, /float get_level\(\) override \{ return this->level; \}/);
    assert.match(types, /float get_level\(\) override \{ return this->level\(\); \}/);
    assert.match(file('yard_query.cpp'), /g->get_level\(\) \+ g->read\(scale\)/);
  });

  it('checks classes against the source with no game, and catches a wrong member', (t) => {
    const cxx = cxxPath();
    const work = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-objects-'));
    after(() => removeTree(work));
    const base = { ts: loadTypeScript(root, typescriptDir()), tsconfig: path.join(root, 'tsconfig.json'), sourceRoot: root, includes: [path.join(target, 'native/src')], cxx, steps: 300, seed: 7,
      tolerance: 1e-4, valueTypes: { V2: { x: 'x', y: 'y' } } };
    const same = runObjectDifferential({ ...base, translation: result, work: path.join(work, 'same') });
    assert.deepEqual(same.mismatches, []);
    assert.ok(same.calls['Tally.add']! > 0, 'a member of a class was called');
    const query = 'native/src/sim/yard/yard_query.cpp';
    const mutated = new Map(result.files);
    mutated.set(query, mutated.get(query)!.replace('this->total += n;', 'this->total -= n;'));
    assert.notEqual(mutated.get(query), result.files.get(query));
    const wrong = runObjectDifferential({ ...base, translation: { ...result, files: mutated }, work: path.join(work, 'wrong') });
    assert.ok(wrong.mismatches.length > 0, 'the mutation must be found');
  });

  it('writes C++ that compiles without a warning against the target headers', (t) => {
    const cxx = cxxPath();
    const work = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-translate-'));
    after(() => removeTree(work));
    for (const [name, text] of result.files) {
      const out = path.join(work, name);
      fs.mkdirSync(path.dirname(out), { recursive: true });
      fs.writeFileSync(out, text);
    }
    for (const name of result.files.keys()) {
      if (!name.endsWith('.cpp')) continue;
      execFileSync(cxx, ['-std=c++17', '-fsyntax-only', '-Wall', '-Wextra', '-Werror', `-I${path.join(work, 'native/src')}`,
        `-I${path.join(target, 'native/src')}`, path.join(work, name)], { stdio: 'pipe' });
    }
  });

  it('agrees with its source on a random scenario, and the comparison catches a wrong translation', (t) => {
    const cxx = cxxPath();
    const work = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-differential-'));
    after(() => removeTree(work));
    const base = { ts: loadTypeScript(root, typescriptDir()), tsconfig: path.join(root, 'tsconfig.json'), sourceRoot: root,
      empty: 'emptyYard', includes: [path.join(target, 'native/src')], cxx, steps: 400, seed: 7, tolerance: 1e-4,
      valueTypes: { V2: { x: 'x', y: 'y' } } };
    const same = runDifferential({ ...base, translation: result, work: path.join(work, 'same') });
    assert.deepEqual(same.mismatches, []);
    assert.ok(Object.keys(same.calls).length >= 6, `every public function called: ${JSON.stringify(same.calls)}`);
    // One comparison turned around: the edit removes every post but the one asked for.
    const edits = 'native/src/sim/yard/yard_edit.cpp';
    const mutated = new Map(result.files);
    mutated.set(edits, mutated.get(edits)!.replace('return p.id == id;', 'return p.id != id;'));
    assert.notEqual(mutated.get(edits), result.files.get(edits));
    const wrong = runDifferential({ ...base, translation: { ...result, files: mutated }, work: path.join(work, 'wrong') });
    assert.ok(wrong.mismatches.length > 0, 'the mutation must be found');
  });
});
