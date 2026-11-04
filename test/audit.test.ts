// Tests of the audit stage: every rule of the catalog fires on the fixture pattern made for it, rules that lack
// their inputs say so, and severity follows the hot path.
import assert from 'node:assert/strict';
import path from 'node:path';
import { describe, it } from 'node:test';
import { RULES } from '../src/audit/catalog.ts';
import type { AuditOptions } from '../src/audit/context.ts';
import { runAudit } from '../src/audit/run.ts';
import { buildGraph } from '../src/graph/build.ts';

const typescriptDir = process.env['CONVERTER_TYPESCRIPT'];
if (!typescriptDir) throw new Error('set CONVERTER_TYPESCRIPT to a typescript package directory to run these tests');
const ir = buildGraph({ sourceRoot: path.join(import.meta.dirname, 'fixtures', 'basic'), typescriptDir, entries: [], include: [], exclude: [] });

const base: AuditOptions = {
  hot: ['src/world.ts#World.tick'], uiDirs: ['src/ui'], stateDirs: ['src/world.ts'], simDirs: ['src/world.ts'],
  maxFileLines: 25, maxFunctionLines: 8, features: [{ name: 'tick', entry: 'src/world.ts#World.tick' }],
  maxScattering: 0, minWriterFiles: 2, minConstantFiles: 2,
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
    assert.ok(found('scattered-writes').some((f) => f.endsWith('src/world.ts#World.total')));
    assert.deepEqual(found('ui-writes-state'), ['high src/ui/panel.ts#overrideTotal']);
    assert.deepEqual(found('nondeterminism-in-simulation'), ['high src/world.ts#World.tick']);
  });

  it('weighs performance patterns by the hot path', () => {
    assert.deepEqual(found('hot-allocation'), ['high src/world.ts#World.tick']);
    assert.deepEqual(found('queue-shift'), ['low src/patterns.ts#drain']);
    assert.deepEqual(found('object-delete'), ['low src/patterns.ts#forget']);
    assert.deepEqual(found('holey-array'), ['low src/patterns.ts#sized']);
    assert.deepEqual(found('quadratic-proximity'), ['medium src/patterns.ts#nearest']);
    assert.deepEqual(found('worker-copy'), ['medium src/patterns.ts#send']);
    const hot = runAudit(ir, { ...base, hot: ['src/patterns.ts#drain'] });
    assert.ok(hot.findings.some((f) => f.rule === 'queue-shift' && f.severity === 'high'));
  });

  it('measures feature scattering with Eaddy\'s formula', () => {
    const tick = result.features[0]!;
    assert.equal(tick.name, 'tick');
    assert.ok(tick.files >= 3 && tick.degreeOfScattering > 0 && tick.degreeOfScattering < 1);
    assert.ok(found('feature-scattering').length === 1);
  });

  it('says which rules could not run and why, instead of reporting nothing', () => {
    const bare = runAudit(ir, { ...base, uiDirs: [], stateDirs: [], simDirs: [], maxFileLines: undefined, maxFunctionLines: undefined, features: [] });
    const skipped = Object.fromEntries(bare.rules.filter((r) => r.skipped).map((r) => [r.id, r.skipped]));
    assert.deepEqual(Object.keys(skipped).sort(), ['feature-scattering', 'god-file', 'god-function', 'nondeterminism-in-simulation', 'ui-writes-state']);
  });
});
