// Tests of the flag parser shared by every command.
import assert from 'node:assert/strict';
import { describe, it } from 'node:test';
import { ArgsError, parseFlags } from '../src/common/args.ts';

const specs = [
  { name: 'out', help: '', required: true },
  { name: 'chromium-arg', help: '', repeatable: true },
  { name: 'variant', help: '', repeatable: true },
];

describe('flags', () => {
  it('reads --name value and --name=value', () => {
    const flags = parseFlags(['--out', 'a.json', '--variant=low=setPreset(\'low\')'], specs);
    assert.equal(flags.one('out'), 'a.json');
    assert.deepEqual(flags.all('variant'), ['low=setPreset(\'low\')']);
  });

  it('passes values that start with -- through the = form', () => {
    const flags = parseFlags(['--out=x', '--chromium-arg=--use-angle=d3d11', '--chromium-arg=--enable-gpu'], specs);
    assert.deepEqual(flags.all('chromium-arg'), ['--use-angle=d3d11', '--enable-gpu']);
  });

  it('refuses what it cannot read, saying how to write it', () => {
    assert.throws(() => parseFlags(['--out', 'x', '--chromium-arg', '--use-angle=d3d11'], specs), /--chromium-arg=value/);
    assert.throws(() => parseFlags(['--nope=1', '--out', 'x'], specs), ArgsError);
    assert.throws(() => parseFlags(['--out', 'x', '--out', 'y'], specs), /given twice/);
    assert.throws(() => parseFlags([], specs), /missing required flag --out/);
  });
});
