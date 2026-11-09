// The conversion's decisions: which units reach the machine, which stay open and why, and what the quarantine refuses.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { describe, it } from 'node:test';
import type { Finding } from '../src/audit/context.ts';
import { folderName, readCoverage, type Delivered } from '../src/convert/inventory.ts';
import { Ledger, type LedgerUnit } from '../src/convert/ledger.ts';
import { scanFiles, scanUnits } from '../src/convert/quarantine.ts';
import { unproven } from '../src/convert/coverage-proof.ts';
import type { EdgeKind, EdgeRecord, GraphIR, UnitKind, UnitRecord } from '../src/graph/ir.ts';

function unit(id: string, kind: UnitKind, extra: { parent?: string; external?: string[] } = {}): UnitRecord {
  return {
    id, kind, name: id.slice(id.lastIndexOf('#') + 1), file: id.split('#')[0]!, start: { line: 1, column: 0 }, end: { line: 2, column: 0 }, lines: 2, exported: true,
    signature: '', parent: extra.parent ?? null, nested: [],
    facts: { external: (extra.external ?? []).map((key) => ({ key, kind: 'call', count: 1 })), nondeterminism: [], loopAllocations: [], numericLiterals: [], stringLiterals: 0,
      decisions: 0, loops: 0, awaits: 0, bitOperations: 0, isStatic: false, callable: kind === 'function' || kind === 'method', mutableModuleState: false, patterns: [], fingerprints: [] },
  };
}

const edge = (from: string, to: string, kind: EdgeKind = 'call'): EdgeRecord => ({ from, to, kind, count: 1 });
const finding = (u: string, rule: string): Finding => ({ rule, severity: 'medium', unit: u, file: u.split('#')[0]!, line: 1, message: rule });

const A = 'src/world/a.ts';
const clean = `${A}#clean`;
const sick = `${A}#sick`;
const caller = `${A}#caller`;
const holey = `${A}#holey`;
const drawn = 'src/render/a.ts#drawn';
const method = `${A}#Thing.run`;
const owner = `${A}#Thing`;
const shape = `${A}#Shape`;
const field = `${A}#Shape.x`;
const reader = `${A}#reader`;
const heapUser = `${A}#heapUser`;
const heap = 'src/core/heap.ts#Heap';
const delivered = `${A}#delivered`;
const usesDelivered = `${A}#usesDelivered`;
const looping = `${A}#looping`;
const loopCaller = `${A}#loopCaller`;
const typeUser = `${A}#typeUser`;
const foreignWriter = 'src/sim/b.ts#foreignWriter';
const level = `${A}#Level`;
const levelReader = `${A}#levelReader`;

const units = [unit(clean, 'function'), unit(sick, 'function'), unit(caller, 'function'), unit(holey, 'function'), unit(drawn, 'function', { external: ['three:Mesh'] }),
  unit(owner, 'class'), unit(method, 'method', { parent: owner }), unit(shape, 'interface'), unit(field, 'field', { parent: shape }), unit(reader, 'function'),
  unit(heapUser, 'function'), unit(heap, 'class'), unit(delivered, 'function'), unit(usesDelivered, 'function'), unit(looping, 'function'),
  unit(loopCaller, 'function'), unit(typeUser, 'function'), unit(foreignWriter, 'function'), unit(level, 'enum'), unit(levelReader, 'function')];

function decide(findings: readonly Finding[], coverage: ReadonlyMap<string, Delivered> = new Map()): Map<string, LedgerUnit> {
  const graph = { files: [{ path: A, lines: 9 }, { path: 'src/render/a.ts', lines: 9 }, { path: 'src/core/heap.ts', lines: 9 }], units,
    edges: [edge(caller, sick), edge(reader, field, 'read'), edge(heapUser, heap, 'construct'), edge(usesDelivered, delivered), edge(loopCaller, looping),
      edge(typeUser, shape, 'reference'), edge(foreignWriter, field, 'write'), edge(levelReader, level, 'read')] } as unknown as GraphIR;
  const ledger = new Ledger({ graph, audit: { findings: [...findings] }, own: units.map((u) => u.id), sliced: [], coverage, ownedFiles: ['src/core/heap.ts'], ownedNames: [] });
  return new Map(ledger.units.map((u) => [u.id, u]));
}

describe('convert: the ledger', () => {
  // scattered-writes as the audit gives it: on the code that writes the state from outside its owner (audit/state.ts)
  const found = [finding(sick, 'quadratic-proximity'), finding(holey, 'holey-array'), finding(looping, 'hot-allocation'), finding(foreignWriter, 'scattered-writes')];
  const ledger = decide(found);
  const disposition = (id: string): string | undefined => ledger.get(id)?.disposition;

  it('gives the machine what is clean', () => {
    assert.equal(disposition(clean), 'translate');
  });

  it('stops a unit with a wrong methodology nothing judges on the output, and names the correct one', () => {
    assert.equal(disposition(sick), 'method');
    assert.match(ledger.get(sick)!.methods[0]!.correct, /spatial partition/);
  });

  it('leaves a form the quarantine finds in the written C++ to the quarantine, its one owner (an allocation in a loop is a heap allocation only in the C++)', () => {
    assert.equal(disposition(looping), 'translate');
    assert.equal(disposition(loopCaller), 'translate');
    assert.match(ledger.get(looping)!.why, /judged on the C\+\+ it becomes/);
  });

  it('stops the code that writes state from outside its owner, and neither the state nor what only refers to it', () => {
    assert.equal(disposition(foreignWriter), 'method');
    assert.equal(disposition(shape), 'translate');
    assert.equal(disposition(typeUser), 'translate');
  });

  it('gives the machine an enum, which it translates, and what reads one', () => {
    assert.equal(disposition(level), 'translate');
    assert.equal(disposition(levelReader), 'translate');
  });

  it('stops what reaches a stopped unit: the disease does not travel through a call', () => {
    assert.equal(disposition(caller), 'method');
    assert.match(ledger.get(caller)!.why, /reaches .*sick/);
  });

  it('lets through only what the translator writes correctly', () => {
    assert.equal(disposition(holey), 'translate');
  });

  it('never gives drawing to the machine, and gives it a class with its members', () => {
    assert.equal(disposition(drawn), 'write');
    assert.equal(disposition(method), 'translate');
  });

  it('does not stop a function for reading a field of a plain type', () => {
    assert.equal(disposition(reader), 'translate');
  });

  it('calls what the library says the game owns, instead of following it', () => {
    assert.equal(disposition(heapUser), 'translate');
  });

  it('marks what a port delivered, and stops what would translate it again', () => {
    const covered = decide([], new Map([[delivered, { port: 'roads', target: 'sim/x', state: 'portado' }]]));
    assert.equal(covered.get(delivered)?.disposition, 'exists');
    assert.equal(covered.get(usesDelivered)?.disposition, 'method');
  });
});

describe('convert: the quarantine', () => {
  const limits = { maxLines: { '.cpp': 10 }, maxFunctionLines: { '.cpp': 4 } };
  const rules = (text: string): string[] => scanFiles(new Map([['a.cpp', text]]), limits).map((f) => f.rule);

  it('accepts clean code', () => {
    assert.deepEqual(rules('int f(int a) {\n\treturn a;\n}\n'), []);
  });

  it('refuses each wrong form', () => {
    assert.deepEqual(rules('q.erase(q.begin());'), ['queue-shift']);
    assert.deepEqual(rules('x = /*untranslated*/;'), ['untranslated']);
    assert.deepEqual(rules('auto *p = new Thing();'), ['hot-allocation']);
    assert.deepEqual(rules('auto t = std::chrono::steady_clock::now();'), ['nondeterminism-in-simulation']);
  });

  it('holds the target\'s size limits', () => {
    assert.deepEqual(rules('int f() {\n\ta;\n\tb;\n\tc;\n\td;\n}\n'), ['god-function']);
    assert.deepEqual(rules('\n'.repeat(12)), ['god-file']);
  });

  it('refuses a stopped unit the translator traced, whatever its roots were', () => {
    const emitted = [{ source: 'a#sick', kind: 'query' }, { source: 'a#ok', kind: 'edit' }, { source: 'NAME', kind: 'constant' }];
    const found = scanUnits((id) => (id === 'a#sick' ? 'wrong methodology' : null), emitted);
    assert.deepEqual(found.map((f) => f.file), ['a#sick']);
  });
});

describe('convert: what is known first', () => {
  it('names a feature folder from its inventory id', () => {
    assert.equal(folderName('editor:poleRoutes'), 'editor-pole-routes');
  });

  it('reads whole files and declarations from a coverage table (coverage.md, or the older cobertura.md), and nothing from a partial row', () => {
    const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-coverage-'));
    fs.mkdirSync(path.join(dir, 'roads'));
    fs.mkdirSync(path.join(dir, 'older'));
    fs.writeFileSync(path.join(dir, 'older', 'cobertura.md'), '| `src/e.ts` | whole 4 | what | `sim/e.cpp` | portado | note |');
    fs.writeFileSync(path.join(dir, 'roads', 'coverage.md'), [
      '| `src/a.ts` | whole 10 | what | `sim/a.cpp` | portado | note |',
      '| `src/b.ts` | part 10 | what | `sim/b.cpp` | portado | note |',
      '| `src/c.ts#x` | function | what | `sim/c.cpp` | j\u00e1 existia | note |',
      '| `src/d.ts` | whole 10 | what | `sim/d.cpp` | a portar | note |',
    ].join('\n'));
    const coverage = readCoverage(dir);
    fs.rmSync(dir, { recursive: true, force: true });
    assert.deepEqual([...coverage.keys()].sort(), ['src/a.ts', 'src/c.ts#x', 'src/e.ts']);
    assert.equal(coverage.get('src/e.ts')?.port, 'older');
    assert.equal(coverage.get('src/a.ts')?.target, 'sim/a.cpp');
  });
});

describe('convert: a coverage claim the game must show', () => {
  it('stands only when the owner files exist and cite the unit, by its name, the game\'s form of it or its 4.0 file', () => {
    const game = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-proof-'));
    fs.mkdirSync(path.join(game, 'native', 'src', 'sim'), { recursive: true });
    fs.writeFileSync(path.join(game, 'native', 'src', 'sim', 'poles.cpp'), '// Roadcraft 4.0 editor/poles.ts, its behaviour\nint plan_run(int a) { return a; }\n');
    const claim = (target: string): Delivered => ({ port: 'poles', target, state: 'portado' });
    const proof = (id: string, target: string): string | null => unproven(game, { id, delivered: claim(target) });
    try {
      assert.equal(proof('src/editor/poles.ts#planRun', '`sim/poles.cpp`'), null, 'cited in the game\'s form');
      assert.equal(proof('src/editor/poles.ts', 'sim/poles.cpp'), null, 'a whole file cited by its 4.0 path');
      assert.match(proof('src/editor/poles.ts#routeAround', 'sim/poles.cpp')!, /never names routeAround \(nor route_around\)/);
      assert.match(proof('src/editor/poles.ts#planRun', 'sim/poles.cpp + render/wires.cpp')!, /the game has no render\/wires\.cpp/);
      assert.match(proof('src/editor/poles.ts#planRun', 'porte demolition')!, /names no file of the game/);
      assert.match(proof('src/render/poleLayer.ts', 'sim/poles.cpp')!, /never cites render\/poleLayer\.ts/);
    } finally {
      fs.rmSync(game, { recursive: true, force: true });
    }
  });
});
