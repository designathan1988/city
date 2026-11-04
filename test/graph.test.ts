// Tests of the graph stage on a fixture that holds, in small, every pattern the real source has at scale.
// Needs CONVERTER_TYPESCRIPT: the directory of a `typescript` package (5.8 or newer).
import assert from 'node:assert/strict';
import path from 'node:path';
import { describe, it } from 'node:test';
import { buildGraph } from '../src/graph/build.ts';
import type { EdgeKind, GraphIR } from '../src/graph/ir.ts';

const typescriptDir = process.env['CONVERTER_TYPESCRIPT'];
if (!typescriptDir) throw new Error('set CONVERTER_TYPESCRIPT to a typescript package directory to run these tests');
const fixture = path.join(import.meta.dirname, 'fixtures', 'basic');

function graph(entries: string[]): GraphIR {
  return buildGraph({ sourceRoot: fixture, typescriptDir, entries, include: [], exclude: [] });
}

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
    const literals = unit(ir, 'src/lib/math.ts#SPEED_LIMIT').facts.numericLiterals;
    assert.deepEqual(literals.map((l) => [l.text, l.trivial]), [['13.9', false]]);
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
