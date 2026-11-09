// A feature's modules share one solved program: that must change nothing they produce, and a module's refusal must
// not pass to the next one.
import assert from 'node:assert/strict';
import path from 'node:path';
import { describe, it } from 'node:test';
import { prepareTranslation, translate } from '../src/translate/run.ts';
import { readTranslationSpec } from '../src/translate/spec.ts';
import { typescriptDir } from './support.ts';

const root = path.join(import.meta.dirname, 'fixtures', 'translate');
const target = path.join(root, 'target');
const spec = readTranslationSpec(path.join(root, 'yard.translate.json'));
const input = { sourceRoot: root, tsconfig: path.join(root, 'tsconfig.json'), typescriptDir: typescriptDir(), targetRoot: target, targetGraph: path.join(target, 'cppgraph.json') };
const bad = { ...spec, source: 'src/world/bad.ts', units: ['addItem'], state: { ...spec.state, source: 'BinData', target: 'BinState', empty: undefined } };

describe('convert: one solved program for a feature', () => {
  const prepared = prepareTranslation({ ...input, spec });

  it('writes the same files as a translation on its own', () => {
    const alone = translate({ ...input, spec });
    const shared = translate({ ...input, spec, prepared });
    assert.deepEqual([...shared.files], [...alone.files]);
    assert.deepEqual(shared.errors, alone.errors);
  });

  it('keeps each module\'s refusal to itself', () => {
    const refused = translate({ ...input, spec: bad, prepared });
    assert.ok(refused.errors.some((e) => e.includes('the edit addItem reads an index')), refused.errors.join('\n'));
    assert.deepEqual(translate({ ...input, spec, prepared }).errors, []);
  });

  it('refuses the same module twice: an error named before is not lost', () => {
    const first = translate({ ...input, spec: bad, prepared });
    const second = translate({ ...input, spec: bad, prepared });
    assert.deepEqual(second.errors, first.errors);
  });
});
