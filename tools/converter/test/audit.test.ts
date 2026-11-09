// Tests of the audit stage: every rule of the catalog fires on the fixture pattern made for it, rules that lack
// their inputs say so, and severity follows the hot path.
import assert from 'node:assert/strict';
import { describe, it } from 'node:test';
import { RULES } from '../src/audit/catalog.ts';
import type { AuditOptions } from '../src/audit/context.ts';
import { literalValue } from '../src/audit/architecture.ts';
import { runAudit } from '../src/audit/run.ts';
import { fixtureGraph } from './support.ts';

const ir = fixtureGraph();

const base: AuditOptions = {
  hot: ['src/world.ts#World.tick'], uiDirs: ['src/ui'], stateDirs: ['src/world.ts'], simDirs: ['src/world.ts'],
  maxFileLines: 25, maxFunctionLines: 8, features: [{ name: 'tick', entry: 'src/world.ts#World.tick' }],
  maxScattering: 0, writerDirs: [], channels: [], stateRoot: null, numberExempt: [], minConstantFiles: 2, allowedNumbers: [0, 1, -1, 2, 0.5],
};

describe('audit', () => {
  const result = runAudit(ir, base);
  const found = (rule: string): string[] => result.findings.filter((f) => f.rule === rule).map((f) => `${f.severity} ${f.unit ?? f.file}`);

  it('has a source for every rule', () => {
    for (const rule of RULES) assert.ok(rule.sources.length > 0 && rule.sources.every((s) => /^https:\/\//.test(s.url)), rule.id);
  });

  it('finds architecture problems', () => {
    assert.ok(found('god-function').includes('medium src/world.ts#World.tick'));
    assert.ok(result.findings.some((f) => f.rule === 'god-file' && f.file === 'src/world.ts'));
    assert.ok(found('duplicated-code').some((f) => f.endsWith('src/dupA.ts#blendA')));
    assert.ok(result.findings.some((f) => f.rule === 'duplicated-constant' && f.message.startsWith('0.25 ')));
  });

  it('finds state without an owner', () => {
    assert.ok(found('module-state').some((f) => f.endsWith('src/lib/math.ts#counter')));
    // the finding is the write from outside the owner: it is on the code that writes, never on the state written (which
    // has no behaviour, and whose readers did nothing wrong)
    assert.ok(found('scattered-writes').some((f) => f.endsWith('src/ui/panel.ts#overrideTotal')));
    assert.ok(!found('scattered-writes').some((f) => f.endsWith('src/world.ts#World.total')));
    assert.ok(result.findings.some((f) => f.rule === 'scattered-writes' && f.message.includes('World.total')), 'it names the state written');
    // A folder the architecture designates as a writer (or a designated channel) is not outside the owner.
    const designated = runAudit(ir, { ...base, writerDirs: ['src/ui'] });
    assert.ok(!designated.findings.some((f) => f.rule === 'scattered-writes' && f.unit === 'src/ui/panel.ts#overrideTotal'));
    const channel = runAudit(ir, { ...base, channels: ['src/world.ts#World'] });
    assert.ok(!channel.findings.some((f) => f.rule === 'scattered-writes' && f.unit === 'src/ui/panel.ts#overrideTotal'));
    assert.deepEqual(found('ui-writes-state'), ['high src/ui/panel.ts#overrideTotal']);
    assert.deepEqual(found('nondeterminism-in-simulation'), ['high src/world.ts#World.tick']);
  });

  it('weighs performance patterns by reachability from the loops, never claiming a measurement', () => {
    assert.deepEqual(found('hot-allocation'), ['medium src/world.ts#World.tick']);
    assert.deepEqual(found('queue-shift'), ['low src/patterns.ts#drain']);
    assert.deepEqual(found('object-delete'), ['low src/patterns.ts#forget']);
    assert.deepEqual(found('holey-array'), ['low src/patterns.ts#sized']);
    assert.deepEqual(found('quadratic-proximity'), ['medium src/patterns.ts#nearest']);
    assert.deepEqual(found('worker-copy'), ['medium src/patterns.ts#send']);
    const hot = runAudit(ir, { ...base, hot: ['src/patterns.ts#drain'] });
    assert.ok(hot.findings.some((f) => f.rule === 'queue-shift' && f.severity === 'medium' && f.message.includes('not measured')));
    assert.ok(!hot.findings.some((f) => f.rule === 'hot-allocation' && f.severity === 'high'));
  });

  it('roots the hot path in repeating frame callbacks, not in one-off deferrals', () => {
    const auto = runAudit(ir, { ...base, hot: [] });
    assert.deepEqual(auto.hot.roots, ['src/loop.ts#Clock.shown']);
    const members = new Set(auto.hot.members);
    for (const id of ['src/loop.ts#Clock.shown', 'src/loop.ts#frame', 'src/loop.ts#step', 'src/loop.ts#Clock.request']) {
      assert.ok(members.has(id), `${id} runs every frame`);
    }
    assert.ok(!members.has('src/loop.ts#deferOnce'), 'a one-off deferral does not run every frame');
  });

  it('measures feature scattering with Eaddy\'s formula', () => {
    const tick = result.features[0]!;
    assert.equal(tick.name, 'tick');
    assert.ok(tick.files >= 3 && tick.degreeOfScattering > 0 && tick.degreeOfScattering < 1);
    assert.ok(found('feature-scattering').length === 1);
  });

  it('says which rules could not run and why, instead of reporting nothing', () => {
    const bare = runAudit(ir, {
      ...base, uiDirs: [], stateDirs: [], simDirs: [], maxFileLines: undefined, maxFunctionLines: undefined, features: [], allowedNumbers: null,
    });
    const skipped = Object.fromEntries(bare.rules.filter((r) => r.skipped).map((r) => [r.id, r.skipped]));
    assert.deepEqual(Object.keys(skipped).sort(),
      ['duplicated-constant', 'feature-scattering', 'god-file', 'god-function', 'nondeterminism-in-simulation', 'ui-writes-state']);
  });

  it('takes the numbers allowed bare from the target\'s rules, and never flags a named constant', () => {
    const strict = runAudit(ir, { ...base, allowedNumbers: [0, 1, -1, 2, 0.5, 0.25, 0.125] });
    assert.ok(!strict.findings.some((f) => f.rule === 'duplicated-constant' && /^0\.(25|125) /.test(f.message)));
    assert.ok(!runAudit(ir, base).findings.some((f) => f.rule === 'duplicated-constant' && f.message.startsWith('13.9 ')));
  });

  it('reads literal values of both languages', () => {
    assert.deepEqual(['0.f', '1.0F', '.5f', '-0.25', "1'000", '1_000', '0x1F', '0xffu', '10ULL', '0b101', '2e3f', 'x'].map(literalValue),
      [0, 1, 0.5, -0.25, 1000, 1000, 31, 255, 10, 5, 2000, NaN]);
  });
});
