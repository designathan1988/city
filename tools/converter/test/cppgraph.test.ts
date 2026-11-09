// Tests of the C++ graph on a fixture subsystem, through a real clangd (the machine file's clangd and compiler driver).
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { after, describe, it } from 'node:test';
import { runAudit } from '../src/audit/run.ts';
import { removeTree } from '../src/common/files.ts';
import { buildCppGraph } from '../src/cppgraph/build.ts';
import type { EdgeKind, GraphIR } from '../src/graph/ir.ts';
import { clangdPath, cxxPath } from './support.ts';

const clangd = clangdPath();
const cxx = cxxPath();
const root = path.join(import.meta.dirname, 'fixtures', 'cpp');

function listing(dir: string): string[] {
  return fs.readdirSync(dir, { recursive: true }).map(String).sort();
}

describe('C++ graph', async () => {
  const work = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-test-cpp-'));
  after(() => removeTree(work));
  const commands = path.join(work, 'compile_commands.json');
  fs.writeFileSync(commands, JSON.stringify(['native/src/sim/fleet.cpp', 'native/src/ui/panel.cpp'].map((file) => ({
    directory: root, file, arguments: [cxx, '-std=c++17', '-Inative/src', '-c', file],
  }))));
  const filesBefore = listing(root);
  const ir: GraphIR = await buildCppGraph({
    root, compileCommands: commands, clangd, cacheDir: path.join(work, 'cache'), include: [], exclude: [], entries: [],
    clangdArgs: [`--query-driver=${cxx.replace(/\\/g, '/')}`], indexTimeoutSeconds: 120,
  });
  const ids = new Set(ir.units.map((u) => u.id));
  const unit = (id: string) => {
    const found = ir.units.find((u) => u.id === id);
    assert.ok(found, `missing ${id}; have ${[...ids].join(', ')}`);
    return found;
  };
  const kinds = (from: string, to: string): EdgeKind[] => ir.edges.filter((e) => e.from === from && e.to === to).map((e) => e.kind).sort();
  const tick = 'native/src/sim/fleet.cpp#Fleet::tick';

  it('makes units of definitions, class heads, fields and globals', () => {
    assert.equal(ir.language, 'cpp');
    for (const id of [tick, 'native/src/sim/fleet.cpp#Fleet::drain', 'native/src/sim/fleet.cpp#Cruise::step', 'native/src/sim/fleet.cpp#clampf',
      'native/src/sim/fleet.h#Fleet', 'native/src/sim/fleet.h#Fleet::total', 'native/src/sim/fleet.h#g_counter']) unit(id);
    assert.equal(unit('native/src/sim/fleet.cpp#Fleet::drain').parent, 'native/src/sim/fleet.h#Fleet');
  });

  it('makes the definition the unit of a class, and gives each member its own class', () => {
    assert.ok(!ids.has('native/src/sim/fleet.h#World'), 'a forward declaration is not a unit');
    assert.equal(unit('native/src/sim/state/world.h#World::money').parent, 'native/src/sim/state/world.h#World');
    assert.equal(unit('native/src/ui/panel.cpp#Car::seats').parent, 'native/src/ui/panel.cpp#Car');
    assert.equal(unit('native/src/sim/fleet.h#Car::s').parent, 'native/src/sim/fleet.h#Car');
  });

  it('types calls, field and global writes, and virtual dispatch', () => {
    assert.ok(kinds('native/src/sim/fleet.cpp#Cruise::step', 'native/src/sim/fleet.cpp#clampf').includes('call'));
    assert.ok(kinds('native/src/sim/fleet.cpp#Cruise::step', 'native/src/sim/fleet.h#SPEED_LIMIT').includes('read'));
    assert.deepEqual(kinds(tick, 'native/src/sim/fleet.h#Fleet::total'), ['read', 'write']);
    assert.deepEqual(kinds(tick, 'native/src/sim/fleet.h#g_counter'), ['read', 'write']);
    assert.ok(kinds(tick, 'native/src/sim/fleet.cpp#Cruise::step').includes('implementation'));
    assert.ok(kinds('native/src/sim/fleet.cpp#Fleet::drain', 'native/src/sim/fleet.h#Fleet::queue').includes('write'));
  });

  it('tells a write to a copy from a write to state', () => {
    const probe = 'native/src/sim/fleet.cpp#Fleet::probe';
    assert.ok(!kinds(probe, 'native/src/sim/fleet.h#Car::v').includes('write'), 'a field of a local copy is not state');
    assert.ok(kinds(probe, 'native/src/sim/fleet.h#Car::s').includes('write'), 'a field through a reference is state');
  });

  it('records facts, patterns and the standard library as external', () => {
    assert.ok(unit(tick).facts.loopAllocations.some((a) => a.what === 'std::vector local'));
    assert.ok(unit(tick).facts.patterns.some((p) => p.id === 'quadratic-proximity'));
    assert.ok(unit('native/src/sim/fleet.cpp#Fleet::drain').facts.patterns.some((p) => p.id === 'queue-shift'));
    assert.equal(unit('native/src/sim/fleet.h#g_counter').facts.mutableModuleState, true);
    assert.equal(unit('native/src/sim/fleet.h#SPEED_LIMIT').facts.mutableModuleState, false);
    assert.equal(unit('native/src/sim/fleet.h#t_inside').facts.mutableModuleState, false);
    assert.ok(unit('native/src/sim/fleet.h#SPEED_LIMIT').facts.numericLiterals.every((l) => l.context === 'constant'));
    assert.ok(unit('native/src/sim/fleet.cpp#Fleet::drain').facts.external.some((e) => e.key.startsWith('std:') && e.key.endsWith('erase')));
  });

  it('is audited by the same catalog, the engine\'s _process rooting the frame path', () => {
    const result = runAudit(ir, {
      hot: [], uiDirs: [], stateDirs: [], simDirs: [], maxFileLines: undefined, maxFunctionLines: undefined, features: [],
      maxScattering: 0.5, writerDirs: [], channels: [], stateRoot: null, numberExempt: [], minConstantFiles: 3, allowedNumbers: null,
    });
    assert.deepEqual(result.hot.roots, ['native/src/sim/fleet.cpp#Driver::_process']);
    const of = (rule: string) => result.findings.filter((f) => f.rule === rule).map((f) => `${f.severity} ${f.unit}`);
    assert.deepEqual(of('hot-allocation'), [`medium ${tick}`]);
    assert.deepEqual(of('quadratic-proximity'), [`medium ${tick}`]);
    assert.deepEqual(of('queue-shift'), ['low native/src/sim/fleet.cpp#Fleet::drain']);
    assert.ok(of('module-state').some((f) => f.endsWith('fleet.h#g_counter')));
  });

  it('owns state by the subsystem holding it under the state root, and leaves held values alone', () => {
    const options = {
      hot: [], uiDirs: [], stateDirs: [], simDirs: [], maxFileLines: undefined, maxFunctionLines: undefined, features: [],
      maxScattering: 0.5, writerDirs: [], channels: [], numberExempt: [], minConstantFiles: 3, allowedNumbers: null,
    };
    const scattered = (stateRoot: string | null) => runAudit(ir, { ...options, stateRoot }).findings
      .filter((f) => f.rule === 'scattered-writes').map((f) => f.message).sort();
    const world = 'native/src/sim/state/world.h#World';
    const tree = scattered(world);
    // each write from outside the owner, on the code that writes (reset_from_panel, in ui), naming the state it writes
    assert.deepEqual(tree, [
      'reset_from_panel writes Fleet::total (owned by native/src/sim) from native/src/ui; 1 owner(s) outside write it: native/src/ui',
      'reset_from_panel writes Letter::code (owned by native/src/sim/state) from native/src/ui; 1 owner(s) outside write it: native/src/ui',
      'reset_from_panel writes World::money (owned by native/src/sim/state) from native/src/ui; 1 owner(s) outside write it: native/src/ui',
    ], 'Counter::n, held by sim but written only by its own Counter::bump (core), is not scattered');
    assert.ok(kinds('native/src/core/counter.h#Counter::bump', 'native/src/core/counter.h#Counter::n').includes('write'));
    const channel = runAudit(ir, { ...options, stateRoot: world, channels: ['native/src/sim/state/world.h#Mailbox'] }).findings
      .filter((f) => f.rule === 'scattered-writes').map((f) => f.message);
    assert.ok(channel.length > 0 && !channel.some((m) => m.includes('writes Letter::code')), 'what a channel holds is the channel');
    assert.ok(scattered(null).some((m) => m.includes('writes Tally::count')), 'without a root, a held value is owned by the folder declaring it');
    const missing = runAudit(ir, { ...options, stateRoot: 'native/src/sim/state/world.h#Nothing' });
    assert.match(missing.rules.find((r) => r.id === 'scattered-writes')!.skipped ?? '', /is not a unit of the graph/);
  });

  it('writes nothing into the analysed project', () => {
    assert.deepEqual(listing(root), filesBefore);
  });
});
