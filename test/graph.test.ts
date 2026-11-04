// Tests of the graph stage on a fixture that holds, in small, every pattern the real source has at scale.
import assert from 'node:assert/strict';
import { describe, it } from 'node:test';
import type { EdgeKind, GraphIR } from '../src/graph/ir.ts';
import { fixtureGraph as graph } from './support.ts';

function kinds(ir: GraphIR, from: string, to: string): EdgeKind[] {
  return ir.edges.filter((e) => e.from === from && e.to === to).map((e) => e.kind).sort();
}

function unit(ir: GraphIR, id: string) {
  const found = ir.units.find((u) => u.id === id);
  assert.ok(found, `missing unit ${id}`);
  return found;
}

describe('graph from an entry', () => {
  const ir = graph(['src/main.ts:start']);

  it('closes over everything the entry uses, implementations and loaded modules included', () => {
    const ids = new Set(ir.units.map((u) => u.id));
    for (const id of [
      'src/main.ts#start', 'src/world.ts#World', 'src/world.ts#World.tick', 'src/world.ts#World.listen',
      'src/world.ts#World.shapes', 'src/world.ts#World.position', 'src/world.ts#World.total', 'src/shape.ts#Shape',
      'src/shape.ts#Square', 'src/shape.ts#Square.area', 'src/shape.ts#Circle', 'src/shape.ts#Circle.area',
      'src/lib/math.ts#clamp', 'src/lib/math.ts#SPEED_LIMIT', 'src/lib/math.ts#counter', 'src/lib/math.ts#bump',
      'src/sim.worker.ts#ready', 'src/lazy.ts#lazyAnswer',
    ]) {
      assert.ok(ids.has(id), `missing ${id}`);
    }
  });

  it('types every edge', () => {
    // `new World()` constructs it; `start(): World` uses it as the return type.
    assert.deepEqual(kinds(ir, 'src/main.ts#start', 'src/world.ts#World'), ['construct', 'typeRef']);
    assert.deepEqual(kinds(ir, 'src/main.ts#start', 'src/world.ts#World.tick'), ['call']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.tick', 'src/shape.ts#Shape'), ['call']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.tick', 'src/shape.ts#Square.area'), ['implementation']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.tick', 'src/shape.ts#Circle.area'), ['implementation']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.tick', 'src/lib/math.ts#clamp'), ['call']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.tick', 'src/lib/math.ts#SPEED_LIMIT'), ['read']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.tick', 'src/world.ts#World.position'), ['read', 'write']);
    assert.deepEqual(kinds(ir, 'src/lib/math.ts#bump', 'src/lib/math.ts#counter'), ['read', 'write']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.listen', 'src/world.ts#World.shapes'), ['read', 'write']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.listen', 'src/world.ts#World.total'), ['write']);
    assert.deepEqual(kinds(ir, 'src/world.ts#World.listen', 'src/shape.ts#Shape'), ['typeRef', 'write']);
    assert.deepEqual(kinds(ir, 'src/shape.ts#Square', 'src/shape.ts#Shape'), ['implements']);
    assert.deepEqual(kinds(ir, 'src/main.ts#start', 'src/lazy.ts#lazyAnswer'), ['moduleLoad']);
  });

  it('records workers and dynamic imports', () => {
    assert.deepEqual(
      ir.moduleLoads.map((m) => [m.from, m.specifier, m.how, m.file?.endsWith('.ts')]),
      [
        ['src/main.ts#start', './lazy', 'dynamicImport', true],
        ['src/main.ts#start', './sim.worker.ts', 'worker', true],
      ],
    );
  });

  it('keys external APIs by their declaring library and container', () => {
    const tick = unit(ir, 'src/world.ts#World.tick');
    const used = tick.facts.external.map((e) => `${e.key}/${e.kind}`);
    for (const key of ['fakelib:Vec/construct', 'fakelib:Vec.add/call', 'fakelib:noise/call', 'es:Math.random/call', 'dom:Performance.now/call']) {
      assert.ok(used.includes(key), `World.tick should use ${key}; has ${used.join(', ')}`);
    }
    assert.deepEqual(tick.facts.nondeterminism, ['dom:Performance.now', 'es:Math.random']);
    assert.ok(ir.external.some((e) => e.key === 'fakelib:Vec' && e.module === 'fakelib' && e.symbol === 'Vec'));
  });

  it('records the facts the later stages decide on', () => {
    const tick = unit(ir, 'src/world.ts#World.tick');
    assert.deepEqual(tick.facts.loopAllocations.map((a) => a.what), ['object literal']);
    assert.equal(tick.facts.loops, 1);
    assert.equal(tick.facts.callable, true);
    assert.equal(unit(ir, 'src/lib/math.ts#counter').facts.mutableModuleState, true);
    assert.equal(unit(ir, 'src/lib/math.ts#SPEED_LIMIT').facts.mutableModuleState, false);
    assert.equal(unit(ir, 'src/world.ts#World.tick').parent, 'src/world.ts#World');
    assert.equal(unit(ir, 'src/lib/math.ts#clamp').facts.decisions, 2);
    // A named constant's value is its owner, not a bare number; inside an expression a number is a value.
    const literals = unit(ir, 'src/lib/math.ts#SPEED_LIMIT').facts.numericLiterals;
    assert.deepEqual(literals.map((l) => [l.text, l.context]), [['13.9', 'constant']]);
    assert.ok(unit(graph(), 'src/dupA.ts#blendA').facts.numericLiterals.some((l) => l.text === '0.25' && l.context === 'value'));
  });
});

describe('factories', () => {
  const ir = graph(['src/main.ts:start']);

  it('makes each inner function a unit, so one use does not pull the whole factory', () => {
    const ids = new Set(ir.units.map((u) => u.id));
    assert.ok(ids.has('src/factory.ts#createCounter.add'));
    assert.ok(!ids.has('src/factory.ts#createCounter.unused'), 'an inner function nobody uses stays out');
    assert.deepEqual(kinds(ir, 'src/main.ts#start', 'src/factory.ts#createCounter.add'), ['call']);
    assert.equal(unit(ir, 'src/factory.ts#createCounter.add').parent, 'src/factory.ts#createCounter');
  });

  it('turns captured closure state into reads and writes of the factory', () => {
    assert.deepEqual(kinds(ir, 'src/factory.ts#createCounter.add', 'src/factory.ts#createCounter'), ['read', 'write']);
  });

  it('keeps nested code out of the factory\'s own facts and marks its ranges', () => {
    const factory = unit(ir, 'src/factory.ts#createCounter');
    assert.equal(factory.nested.length, 4); // add, reset, unused, read()
    assert.equal(factory.facts.numericLiterals.some((l) => l.text === '99'), false);
  });

  it('names the methods of the returned object after their factory', () => {
    const whole = graph([]);
    assert.ok(whole.units.some((u) => u.id === 'src/factory.ts#createCounter.read' && u.kind === 'method'));
    assert.ok(whole.units.some((u) => u.id === 'src/factory.ts#createCounter.unused'));
  });
});

describe('callback flow and scheduling', () => {
  const ir = graph([]);

  it('links a call through a parameter property to the function passed into it', () => {
    assert.deepEqual(kinds(ir, 'src/loop.ts#Clock.shown', 'src/loop.ts#frame'), ['indirectCall']);
  });

  it('records what is handed to the frame scheduler, inline callbacks included', () => {
    assert.ok(kinds(ir, 'src/loop.ts#Clock.request', 'src/loop.ts#Clock.shown').includes('schedule'));
    assert.ok(kinds(ir, 'src/loop.ts#deferOnce', 'src/loop.ts#step').includes('schedule'));
  });
});

describe('patterns and fingerprints', () => {
  const ir = graph([]);
  const patterns = (id: string): string[] => unit(ir, id).facts.patterns.map((p) => p.id);

  it('finds each methodology pattern where it is', () => {
    assert.deepEqual(patterns('src/patterns.ts#drain'), ['queue-shift']);
    assert.deepEqual(patterns('src/patterns.ts#forget'), ['object-delete']);
    assert.deepEqual(patterns('src/patterns.ts#sized'), ['holey-array']);
    assert.deepEqual(patterns('src/patterns.ts#nearest'), ['quadratic-proximity']);
    assert.deepEqual(patterns('src/patterns.ts#send'), ['worker-copy']);
  });

  it('gives identical code identical fingerprints and different code none in common', () => {
    const a = new Set(unit(ir, 'src/dupA.ts#blendA').facts.fingerprints.map(([h]) => h));
    const b = unit(ir, 'src/dupB.ts#blendB').facts.fingerprints.map(([h]) => h);
    assert.ok(a.size > 0 && b.some((h) => a.has(h)), 'the duplicated body shares fingerprints');
    const other = unit(ir, 'src/patterns.ts#nearest').facts.fingerprints.map(([h]) => h);
    assert.ok(!other.some((h) => a.has(h)));
  });
});

describe('graph of the whole scope', () => {
  it('holds every declaration and is byte-identical across runs', () => {
    const first = graph([]);
    const second = graph([]);
    assert.ok(first.units.some((u) => u.id === 'src/lazy.ts#lazyAnswer'));
    const strip = (ir: GraphIR): string => JSON.stringify({ ...ir, stats: { ...ir.stats, milliseconds: 0 } });
    assert.equal(strip(first), strip(second));
  });

  it('refuses an entry that does not exist', () => {
    assert.throws(() => graph(['src/main.ts:nothing']), /no declaration named nothing/);
  });
});
