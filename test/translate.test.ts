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
import { runDifferential } from '../src/translate/differential.ts';
import { translate } from '../src/translate/run.ts';
import { loadTypeScript } from '../src/graph/typescript.ts';
import { readTranslationSpec } from '../src/translate/spec.ts';
import { typescriptDir } from './support.ts';

const root = path.join(import.meta.dirname, 'fixtures', 'translate');
const target = path.join(root, 'target');
const spec = readTranslationSpec(path.join(root, 'yard.translate.json'));
const input = { sourceRoot: root, tsconfig: path.join(root, 'tsconfig.json'), typescriptDir: typescriptDir(), targetRoot: target };

describe('translate', () => {
  const result = translate({ ...input, spec });
  const file = (name: string): string => result.files.get(`native/src/sim/yard/${name}`) ?? '';
  const all = [...result.files.values()].join('\n');

  it('translates the whole feature', () => {
    assert.deepEqual(result.errors, []);
    assert.deepEqual([...result.files.keys()].sort(), ['yard.h', 'yard_edit.cpp', 'yard_index.cpp', 'yard_query.cpp', 'yard_types.h'].map((f) => `native/src/sim/yard/${f}`));
  });

  it('keeps integers integers, through cycles, callbacks and helpers called with integers', () => {
    assert.match(file('yard_query.cpp'), /\bint n = 0;/);
    assert.match(file('yard_types.h'), /struct Nearest \{\n\tint32_t i = 0;\n\tfloat d = 0;\n\tV2 at;\n\};/);
    assert.match(file('yard_query.cpp'), /std::unordered_map<int64_t, std::vector<int>> at;/);
    assert.match(file('yard_query.cpp'), /auto link = \[&\]\(int32_t a, int32_t b\) -> void/);
    assert.match(file('yard_types.h'), /int32_t next_id = 1;/, 'the empty state starts ids at 1');
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
    assert.match(file('yard_types.h'), /std::unordered_map<int64_t, std::vector<const Post \*>> cells;/);
    assert.match(file('yard_index.cpp'), /index\.cells\[key\]\.push_back\(&p\);/, 'one lookup makes the entry when absent');
  });

  it('rounds the JavaScript way and edits in place', () => {
    assert.match(file('yard_query.cpp'), /std::floor\(x_\d+\); return int\(x_\d+ - down >= 0\.5f \? down \+ 1 : down\)/);
    assert.match(file('yard_edit.cpp'), /t\.posts\.push_back\(Post\{id, x, y, \{\}\}\);/);
    assert.match(file('yard_edit.cpp'), /t\.posts\.erase\(std::remove_if\(t\.posts\.begin\(\), t\.posts\.end\(\), \[&\]\(const Post &p\) \{ return p\.id == id; \}\), t\.posts\.end\(\)\);/);
  });

  it('refuses an edit that reads an index of the state it changes', () => {
    const bad = translate({ ...input, spec: { ...spec, source: 'src/bad.ts', units: ['addItem'], state: { ...spec.state, source: 'BinData', target: 'BinState', empty: undefined } } });
    assert.ok(bad.errors.some((e) => e.includes('the edit addItem reads an index')), bad.errors.join('\n'));
  });

  it('writes C++ that compiles without a warning against the target headers', (t) => {
    const cxx = process.env['CONVERTER_CXX'];
    if (!cxx) return t.skip('set CONVERTER_CXX to compile the translation');
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
    const cxx = process.env['CONVERTER_CXX'];
    if (!cxx) return t.skip('set CONVERTER_CXX to run the translation');
    const work = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-differential-'));
    after(() => removeTree(work));
    const base = { ts: loadTypeScript(root, typescriptDir()), tsconfig: path.join(root, 'tsconfig.json'), sourceFile: path.join(root, 'src/yard.ts'),
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
