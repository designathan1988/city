// Tests of the measure stage's attribution: real source maps (from TypeScript's own transpiler) take each runtime
// function's start back to its unit, telling the unit's own function from closures inside it; and the audit rates
// performance findings from a measured profile.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import { createRequire } from 'node:module';
import { pathToFileURL } from 'node:url';
import { describe, it } from 'node:test';
import { runAudit } from '../src/audit/run.ts';
import { buildGraph } from '../src/graph/build.ts';
import { loadTraceMapping, packageOf, projectFile, ScriptMapper, UnitIndex } from '../src/measure/attribute.ts';
import type { Profile } from '../src/measure/profile.ts';

const typescriptDir = process.env['CONVERTER_TYPESCRIPT'];
if (!typescriptDir) throw new Error('set CONVERTER_TYPESCRIPT to a typescript package directory to run these tests');
const ts = createRequire(path.join(typescriptDir, 'package.json'))(typescriptDir) as typeof import('typescript');
const fixture = path.join(import.meta.dirname, 'fixtures', 'basic');
const ir = buildGraph({ sourceRoot: fixture, typescriptDir, entries: [], include: [], exclude: [] });

describe('attribution through source maps', () => {
  const file = path.join(fixture, 'src', 'loop.ts');
  const out = ts.transpileModule(fs.readFileSync(file, 'utf8'), {
    fileName: 'loop.ts',
    compilerOptions: { target: ts.ScriptTarget.ES2022, module: ts.ModuleKind.ESNext, sourceMap: true },
  });
  const js = out.outputText;
  const mapper = new ScriptMapper(loadTraceMapping(typescriptDir), js, JSON.parse(out.sourceMapText!), pathToFileURL(path.join(fixture, 'src', 'loop.js')).href);
  const index = new UnitIndex(ir);
  const attribute = (needle: string) => {
    const offset = js.indexOf(needle);
    assert.ok(offset >= 0, `"${needle}" is in the transpiled script`);
    const original = mapper.original(offset);
    assert.ok(original, `"${needle}" maps back`);
    const relative = projectFile(fixture, 'http://unused', original.source);
    assert.equal(relative, 'src/loop.ts');
    return index.locate(relative, original.at);
  };

  it('finds a function declaration\'s own function', () => {
    assert.deepEqual(attribute('function frame'), { unit: 'src/loop.ts#frame', own: true });
  });

  it('finds a field\'s arrow function as the field unit itself', () => {
    assert.deepEqual(attribute('(now) => {\n        this.frame'), { unit: 'src/loop.ts#Clock.shown', own: true });
  });

  it('tells a closure inside a unit from the unit', () => {
    assert.deepEqual(attribute('() => step(0)'), { unit: 'src/loop.ts#deferOnce', own: false });
  });

  it('names the package of a dependency\'s source', () => {
    assert.equal(packageOf('http://127.0.0.1:5300/node_modules/three/build/three.module.js'), 'three');
    assert.equal(packageOf('http://127.0.0.1:5300/node_modules/@scope/pkg/x.js'), '@scope/pkg');
    assert.equal(packageOf('http://127.0.0.1:5300/node_modules/.vite/deps/three.js'), null);
  });
});

describe('audit with a measured profile', () => {
  const profile: Profile = {
    version: 1, sourceRoot: ir.sourceRoot, scenario: '/', frameUnit: 'src/loop.ts#frame', renderer: 'test', chromiumArgs: [],
    warmupSeconds: 1, windowSeconds: 1, graphUnits: ir.units.length,
    variants: [{
      name: 'low', setup: '', frames: 100, seconds: 1, packages: [], contexts: [], unmapped: { functions: 0, calls: 0 },
      units: [
        { id: 'src/loop.ts#frame', calls: 100, perFrame: 1, closureCalls: 0, contexts: ['page'] },
        { id: 'src/patterns.ts#drain', calls: 300, perFrame: 3, closureCalls: 0, contexts: ['worker'] },
        { id: 'src/patterns.ts#forget', calls: 5, perFrame: 0.05, closureCalls: 0, contexts: ['page'] },
      ],
    }],
  };
  const result = runAudit(ir, {
    hot: [], uiDirs: [], stateDirs: [], simDirs: [], maxFileLines: undefined, maxFunctionLines: undefined, features: [],
    maxScattering: 0.5, minWriterFiles: 3, minConstantFiles: 3, profile,
  });
  const finding = (rule: string) => result.findings.find((f) => f.rule === rule)!;

  it('rates by measured calls per frame', () => {
    assert.equal(finding('queue-shift').severity, 'high');
    assert.match(finding('queue-shift').message, /measured 3\.0 calls per frame, low/);
    assert.equal(finding('object-delete').severity, 'medium');
    assert.equal(finding('holey-array').severity, 'low');
    assert.match(finding('holey-array').message, /not executed in the measured run/);
  });

  it('reports allocations only for code that ran', () => {
    assert.ok(!result.findings.some((f) => f.rule === 'hot-allocation' && f.unit === 'src/world.ts#World.tick'));
  });
});
