// Tests of the classification: roles from each unit's facts, mixed units found, dependencies mapped only to target
// units that exist, and the feature blocked until nothing is unmapped or mixed.
import assert from 'node:assert/strict';
import { describe, it } from 'node:test';
import { classifyFeature } from '../src/classify/run.ts';
import type { GraphIR } from '../src/graph/ir.ts';
import { fixtureGraph } from './support.ts';

const source = fixtureGraph();
/** A target graph holding just these unit ids (the classification reads which target units exist). */
function target(ids: string[]): GraphIR {
  const template = source.units[0]!;
  return { ...source, language: 'cpp', units: ids.map((id) => ({ ...template, id })), edges: [] };
}
const game = target(['native/src/sim/fleet.cpp#clampf', 'native/src/sim/fleet.h#Mover', 'native/src/core/random.h#Rng']);

describe('classify', () => {
  it('gives each unit its role from its facts', () => {
    const c = classifyFeature(source, game, { feature: 'world', subsystem: 'native/src/sim/world', files: ['src/world.ts'] }, { concepts: [] });
    const role = (id: string) => c.units.find((u) => u.id === id)?.role;
    assert.equal(role('src/world.ts#World.tick'), 'tick');
    assert.equal(role('src/world.ts#World.shapes'), 'state');
    assert.equal(role('src/world.ts#World'), 'state');
  });

  it('finds the unit that shows and also changes state, and blocks until it is split', () => {
    const c = classifyFeature(source, game, { feature: 'hud', subsystem: 'project/ui', files: ['src/hud.ts'] }, { concepts: [] });
    const role = (id: string) => c.units.find((u) => u.id === id)?.role;
    assert.equal(role('src/hud.ts#showTotal'), 'ui');
    assert.equal(role('src/hud.ts#resetFromHud'), 'mixed');
    assert.equal(c.ready, false);
    assert.ok(c.blocked.some((b) => b.startsWith('src/hud.ts#resetFromHud: mixes ui and tick')));
  });

  it('maps dependencies only to target units that exist', () => {
    const spec = { feature: 'world', subsystem: 'native/src/sim/world', files: ['src/world.ts'] };
    const c = classifyFeature(source, game, spec, { concepts: [
      { source: 'src/lib/math.ts#clamp', target: 'native/src/sim/fleet.cpp#clampf', reason: 'both clamp a value to a range' },
      { source: 'src/shape.ts#Shape', target: 'native/src/sim/fleet.h#Nothing', reason: 'wrong on purpose' },
    ] });
    const dep = (id: string) => c.dependencies.find((d) => d.source === id)!;
    assert.equal(dep('src/lib/math.ts#clamp').problem, null);
    assert.match(dep('src/shape.ts#Shape').problem!, /target graph does not have/);
    assert.equal(dep('src/lib/math.ts#bump').problem, 'no target owner mapped');
    assert.equal(c.ready, false);
    assert.ok(c.rewrites.some((r) => r.unit === 'src/world.ts#World.tick' && r.rule === 'hot-allocation'));
    assert.ok(c.apis.includes('fakelib:Vec'));
  });

  it('lists the constants that need a source', () => {
    const c = classifyFeature(source, game, { feature: 'math', subsystem: 'native/src/core', files: ['src/lib/math.ts'] }, { concepts: [] });
    assert.deepEqual(c.constants, [{ unit: 'src/lib/math.ts#SPEED_LIMIT', value: '13.9', line: 5 }]);
  });
});
