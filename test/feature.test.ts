// Tests of the feature boundary on a fixture program: a root page script that plugs the feature in, shared code the
// program also uses, a helper only the feature uses (in its own file and inside a shared file), and dead code that
// names the helper (it runs nothing, so it does not make the helper shared).
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { describe, it } from 'node:test';
import { featureBoundary, htmlRoots } from '../src/feature/boundary.ts';
import { writeSources } from '../src/feature/sources.ts';
import { removeTree } from '../src/common/files.ts';
import { buildGraph } from '../src/graph/build.ts';
import { typescriptDir } from './support.ts';

const root = path.join(import.meta.dirname, 'fixtures', 'feature');

describe('feature boundary', () => {
  const ir = buildGraph({ sourceRoot: root, typescriptDir: typescriptDir(), entries: [], include: [], exclude: [] });
  const roots = htmlRoots(root);
  const b = featureBoundary(ir, ['src/feature/ferry.ts'], roots);

  it('starts the program from the scripts its pages load', () => {
    assert.deepEqual(roots, ['src/main.ts']);
  });

  it('owns what only the feature reaches, wherever it is declared; dead code makes nothing shared', () => {
    assert.ok(b.own.includes('src/feature/ferry.ts#Ferry.sail'));
    assert.ok(b.own.includes('src/only/docks.ts#dockNear'), 'a helper only the feature runs');
    assert.ok(b.own.includes('src/shared/math.ts#roundTo'), 'a declaration of a shared file only the feature runs');
    assert.ok(!b.own.includes('src/shared/math.ts#clamp'), 'the program uses it without the feature');
    assert.ok(!b.own.includes('src/only/snap.ts#snap'), 'a file the program loads runs it at load time');
    assert.ok(!b.own.some((id) => id.startsWith('src/dead.ts') || id.startsWith('src/main.ts')));
  });

  it('lists the shared code the feature uses and where the program plugs it in', () => {
    assert.deepEqual(b.uses.map((u) => u.unit), ['src/only/snap.ts#snap', 'src/shared/math.ts#clamp']);
    const hooks = b.hooks.map((h) => `${h.unit} <- ${h.kind}`);
    assert.ok(hooks.includes('src/feature/ferry.ts#Ferry <- construct'), hooks.join('\n'));
    assert.ok(hooks.includes('src/feature/ferry.ts#Ferry.sail <- call'), hooks.join('\n'));
    assert.ok(!b.hooks.some((h) => h.by.some((by) => by.startsWith('src/dead.ts'))), 'dead code is not a place the program plugs in');
  });

  it('writes the code of the feature: whole files it owns, and only its own declarations of a shared file', () => {
    const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-feature-'));
    try {
      const written = writeSources(ir, b, root, dir);
      assert.deepEqual(written.map((w) => `${w.file} ${w.whole ? 'whole' : 'part'}`),
        ['src/feature/ferry.ts whole', 'src/only/docks.ts whole', 'src/shared/math.ts part']);
      const read = (f: string): string => fs.readFileSync(path.join(dir, f), 'utf8');
      assert.equal(read('src/only/docks.ts'), fs.readFileSync(path.join(root, 'src/only/docks.ts'), 'utf8'));
      assert.match(read('src/shared/math.ts'), /export function roundTo/);
      assert.doesNotMatch(read('src/shared/math.ts'), /clamp/, 'the shared declaration stays out');
      assert.equal(read('src/shared/math.ts').match(/function snapTo/g)?.length, 1, 'a nested declaration is written with its parent, once');
    } finally {
      removeTree(dir);
    }
  });

  it('refuses a file that is not in the program', () => {
    assert.throws(() => featureBoundary(ir, ['src/nothing.ts'], roots), /src\/nothing\.ts has no declarations/);
  });
});
