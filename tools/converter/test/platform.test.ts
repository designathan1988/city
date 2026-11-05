// Tests of where the platform calls the feature (src/feature/platform.ts) on a fixture whose package calls the
// program's code: an override of a member its renderer calls (directly and through a class of the program), a
// function stored in a member of the package, one given to a call of the package, and the standard library's
// iterations, which are not hooks.
import assert from 'node:assert/strict';
import path from 'node:path';
import { describe, it } from 'node:test';
import { featureBoundary } from '../src/feature/boundary.ts';
import { holdsFeature } from '../src/feature/holders.ts';
import { castExternals, platformHooks } from '../src/feature/platform.ts';
import { UnitIndex } from '../src/feature/units.ts';
import { buildGraph } from '../src/graph/build.ts';
import { loadTypeScript, openProgram } from '../src/graph/typescript.ts';
import { typescriptDir } from './support.ts';

const root = path.join(import.meta.dirname, 'fixtures', 'platform');

describe('platform hooks', () => {
  const ir = buildGraph({ sourceRoot: root, typescriptDir: typescriptDir(), entries: [], include: [], exclude: [] });
  const ts = loadTypeScript(root, typescriptDir());
  const sp = openProgram(ts, root, path.join(root, 'tsconfig.json'));
  const index = new UnitIndex(sp, ir);
  const b = featureBoundary(ir, ['src/feature/lamps.ts'], ['src/main.ts'], holdsFeature(ts, sp.checker, index, ir));
  const hooks = platformHooks(ts, sp.checker, index, b, sp.root);
  const has = (unit: string, how: string, by: string): boolean => hooks.some((h) => h.unit === unit && h.how === how && h.by === by);

  it('finds an override of a member the package declares, through the program\'s own classes too', () => {
    assert.ok(has('src/feature/lamps.ts#LampRoot.update', 'override', 'fakegl:LOD.update'), JSON.stringify(hooks));
    assert.ok(has('src/feature/lamps.ts#StreetLamps.update', 'override', 'fakegl:LOD.update'), JSON.stringify(hooks));
  });

  it('a member the bases do not declare is not called by the package', () => {
    assert.ok(!hooks.some((h) => h.unit === 'src/feature/lamps.ts#LampRoot.count'));
  });

  it('finds a function stored in a member of the package and one given to a call of the package', () => {
    assert.ok(has('src/feature/lamps.ts#noteScreen', 'callback', 'fakegl:Object3D.onBeforeRender'), JSON.stringify(hooks));
    assert.ok(has('src/feature/lamps.ts#plant', 'callback', 'fakegl:onFrame'), JSON.stringify(hooks));
  });

  it('the standard library\'s iterations run in place: not hooks', () => {
    assert.ok(!hooks.some((h) => h.by.startsWith('es:')), JSON.stringify(hooks));
  });

  it('each hook says where', () => {
    for (const h of hooks) assert.match(h.at, /^src\/feature\/lamps\.ts:\d+$/);
  });

  it('a class whose bases are the package\'s is built by the package: its constructor, or the class', () => {
    assert.ok(has('src/feature/lamps.ts#StreetLamps.constructor', 'override', 'fakegl:LOD.constructor'), JSON.stringify(hooks));
    assert.ok(has('src/feature/lamps.ts#LampRoot', 'override', 'fakegl:LOD.constructor'), JSON.stringify(hooks));
  });

  it('a function given to shared code of the program is an entry: directly, or as a member of an object literal', () => {
    assert.ok(has('src/feature/lamps.ts#plant', 'callback', 'program:src/shared/queue.ts#later'), JSON.stringify(hooks));
    assert.ok(has('src/feature/lamps.ts#plant', 'callback', 'program:src/shared/queue.ts#later2'), JSON.stringify(hooks));
  });

  it('a package\'s object seen through a type of the program: each member of that type is the package\'s, `this` too', () => {
    const casts = castExternals(ts, sp.checker, index, b, sp.root).map((c) => c.key);
    assert.deepEqual(casts, ['fakegl:LOD._depth', 'fakegl:LOD._levels', 'fakegl:LOD._pick']);
  });

  it('a function the feature only forwards to shared code: the function written at the feature\'s call', () => {
    assert.ok(hooks.some((h) => h.unit === 'src/feature/lamps.ts#plant' && h.by === 'program:src/shared/queue.ts#later' && h.at === 'src/feature/lamps.ts:45'), JSON.stringify(hooks));
  });

  it('a class satisfying an interface by its members implements it: its method and its getter run through the interface', () => {
    const impl = (to: string): boolean => ir.edges.some((e) => e.from === 'src/shared/shapes.ts#measure' && e.to === to && e.kind === 'implementation');
    assert.ok(impl('src/feature/lamps.ts#Disc.grow'), JSON.stringify(ir.edges.filter((e) => e.from.includes('measure'))));
    assert.ok(impl('src/feature/lamps.ts#Disc.area'), JSON.stringify(ir.edges.filter((e) => e.from.includes('measure'))));
  });

  it('a global of the platform replaced by a subclass (an interface and a variable of one name, as `Worker`)', () => {
    assert.ok(ir.edges.some((e) => e.from === 'src/shared/patch.ts#startEngine' && e.to === 'src/shared/patch.ts#watchEngines' && e.kind === 'indirectCall'),
      JSON.stringify(ir.edges.filter((e) => e.from.includes('patch.ts'))));
  });

  it('a class written inside a function belongs to it: its members are reached with the function', () => {
    const ctor = ir.units.find((u) => u.id.startsWith('src/shared/patch.ts#Watched.run'));
    assert.equal(ctor?.parent, 'src/shared/patch.ts#watchEngines', JSON.stringify(ir.units.filter((u) => u.file.endsWith('patch.ts')).map((u) => [u.id, u.parent])));
  });

  it('a global of the package the program replaces: every use of it reaches the code that replaced it', () => {
    assert.ok(ir.edges.some((e) => e.from === 'src/shared/patch.ts#useRenderer' && e.to === 'src/shared/patch.ts#patchRenderer' && e.kind === 'indirectCall'),
      JSON.stringify(ir.edges.filter((e) => e.from.includes('patch.ts'))));
  });
});
