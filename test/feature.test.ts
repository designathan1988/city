// Tests of the feature cut on a fixture program: a root page script that plugs the feature in and keeps its values in
// state of its own, shared code the program also uses, helpers only the feature uses (in their own file and inside a
// shared file), dead code that names a helper (it runs nothing), a shared function with the feature inside it, a
// shared dictionary of texts, a shared style sheet and a data file the feature imports.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { describe, it } from 'node:test';
import { removeTree } from '../src/common/files.ts';
import { featureBoundary } from '../src/feature/boundary.ts';
import { CallTargets, runJelly } from '../src/feature/callgraph.ts';
import { PointsTo } from '../src/feature/pointsto.ts';
import { holdsFeature } from '../src/feature/holders.ts';
import { htmlRoots } from '../src/feature/html.ts';
import { writeInterface } from '../src/feature/interface.ts';
import { featureResources } from '../src/feature/resources.ts';
import { sliceShared } from '../src/feature/slice.ts';
import { writeSources } from '../src/feature/sources.ts';
import { UnitIndex } from '../src/feature/units.ts';
import { StringValues } from '../src/feature/values.ts';
import { buildGraph } from '../src/graph/build.ts';
import { loadTypeScript, openProgram } from '../src/graph/typescript.ts';
import { jellyDir, postcssDir, typescriptDir } from './support.ts';

const root = path.join(import.meta.dirname, 'fixtures', 'feature');

describe('feature cut', () => {
  const ir = buildGraph({ sourceRoot: root, typescriptDir: typescriptDir(), entries: [], include: [], exclude: [] });
  const ts = loadTypeScript(root, typescriptDir());
  const sp = openProgram(ts, root, path.join(root, 'tsconfig.json'));
  const index = new UnitIndex(sp, ir);
  const roots = htmlRoots(root);
  const holds = holdsFeature(ts, sp.checker, index, ir);
  const b = featureBoundary(ir, ['src/feature/ferry.ts'], roots, holds);
  const slices = sliceShared(ts, sp.checker, index, b);
  const work = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-jelly-'));
  const jelly = runJelly(jellyDir(), path.join(root, 'src'), roots.map((r) => path.join(root, r)), work);
  const calls = new CallTargets(ts, jelly.callGraph, jelly.base, (file) => sp.sourceFile(file));
  const pts = new PointsTo(ts, jelly.tokens, jelly.base);
  removeTree(work);
  const resources = featureResources(ts, sp.checker, index, root, b, slices, calls, pts, postcssDir());

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
    // main.ts#host: the program implements the feature interface; a call of it runs the program code.
    assert.deepEqual([...new Set(b.uses.map((u) => u.unit))], ['src/main.ts#host', 'src/only/snap.ts#snap', 'src/shared/i18n.ts#t',
      'src/shared/math.ts#clamp', 'src/shared/math.ts#FULL', 'src/shared/math.ts#within']);
    const hooks = b.hooks.map((h) => `${h.unit} <- ${h.kind}`);
    assert.ok(hooks.includes('src/feature/ferry.ts#Ferry <- construct'), hooks.join('\n'));
    assert.ok(hooks.includes('src/feature/ferry.ts#Ferry.sail <- call'), hooks.join('\n'));
    assert.ok(!b.hooks.some((h) => h.by.some((by) => by.startsWith('src/dead.ts'))), 'dead code is not a place the program plugs in');
  });

  it('finds the state of the program that holds the feature values, through copies of copies', () => {
    assert.deepEqual(b.holders, ['src/main.ts#docked', 'src/main.ts#doubled', 'src/main.ts#ferry'],
      'what a shared function returns (report, boxed) is its own value, even when given the feature values');
    assert.ok(b.sliced.includes('src/main.ts#status') && b.sliced.includes('src/main.ts#boxed') && b.sliced.includes('src/main.ts#report'));
    assert.ok(!b.sliced.includes('src/main.ts#level') && !b.sliced.includes('src/main.ts#okText'), 'it reads no value of the feature');
    assert.ok(!b.sliced.some((id) => id.startsWith('src/dead.ts')), 'dead code is not sliced');
  });

  it('slices a shared function: what names the feature, what reads its values, what it controls; not the rest', () => {
    const status = slices.find((s) => s.unit === 'src/main.ts#status');
    assert.ok(status, slices.map((s) => s.unit).join('\n'));
    // 15: the signature (its parameter is a ferry); 18-20: the if on the ferry; 21: what reads the label it sets;
    // 22: what reads the parameter; 24-27: the loop around a statement on the ferry (25 reads what 26 sets, a
    // loop-carried value); 28-29: what reads them, and the end. 16-17 and 23 set values the feature does not reach.
    assert.deepEqual(status.lines, [15, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29]);
  });

  it('finds the entries, style rules and data files the feature uses', () => {
    const of = (kind: string): string[] => resources.filter((r) => r.kind === kind).map((r) => `${r.owner} ${r.key} ${r.file}:${r.line}`);
    assert.deepEqual(of('entry'), ['feature ferry.hint src/shared/i18n.ts:4', 'feature ferry.mode.fast src/shared/i18n.ts:5',
      'feature ferry.mode.slow src/shared/i18n.ts:6', 'shared common.ok src/shared/i18n.ts:8', 'feature ferry.trip.one src/shared/i18n.ts:9',
      'feature ferry.trip.other src/shared/i18n.ts:10', 'feature ferry.board src/shared/i18n.ts:11'],
      'a template expands to the values of its parts types; a key made by a lookup function (plural); one given to an interface whose ' +
      'implementation looks it up (the call graph says which); a key the program also looks up is ' +
      'shared; an object nobody looks up (LEGACY) belongs to nobody, whatever its keys spell');
    assert.deepEqual(of('style'), ['feature ferry-row src/shared/app.css:3', 'feature ferry-row src/shared/app.css:6', 'shared panel src/shared/app.css:8',
      'feature ferry-row src/shared/app.css:9'], 'a rule needing a class the feature never names (.panel .other) stays out');
    assert.deepEqual(of('import'), ['feature ./routes.json src/feature/ferry.ts:6']);
  });

  it('writes the code of the feature: whole files, its declarations and slices of shared files, the files it imports', () => {
    const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-feature-'));
    try {
      const written = writeSources(ir, b, slices, resources, root, dir);
      assert.deepEqual(written.map((w) => `${w.file} ${w.how}`),
        ['src/feature/ferry.ts whole', 'src/feature/routes.json whole', 'src/main.ts part', 'src/only/docks.ts whole', 'src/shared/i18n.ts part',
          'src/shared/math.ts part'],
        'a data file only the feature imports is a module of its own');
      const read = (f: string): string => fs.readFileSync(path.join(dir, f), 'utf8');
      assert.equal(read('src/only/docks.ts'), fs.readFileSync(path.join(root, 'src/only/docks.ts'), 'utf8'));
      assert.match(read('src/shared/math.ts'), /export function roundTo/);
      assert.doesNotMatch(read('src/shared/math.ts'), /clamp/, 'the shared declaration stays out');
      assert.equal(read('src/shared/math.ts').match(/function snapTo/g)?.length, 1, 'a nested declaration is written with its parent, once');
      assert.match(read('src/main.ts'), /export function status\(f: Ferry, n: number\): string \{\n {2}\/\/ … 2 lines\n {2}if \(f\.at > 0\) \{/);
      assert.doesNotMatch(read('src/main.ts'), /const extra/);
      assert.doesNotMatch(read('src/main.ts'), /let total = 0/);
    } finally {
      removeTree(dir);
    }
  });

  it('reads the strings a value can be by the flow of values, not by its type', () => {
    const values = new StringValues(ts, sp.checker, [...index.byFile.keys()].map((f) => index.sourceFile(f)!), calls, pts);
    const main = index.sourceFile('src/main.ts')!;
    const initializer = (name: string): import('typescript').Expression => {
      let found: import('typescript').Expression | undefined;
      const visit = (n: import('typescript').Node): void => {
        if (ts.isVariableDeclaration(n) && ts.isIdentifier(n.name) && n.name.text === name) found = n.initializer;
        ts.forEachChild(n, visit);
      };
      visit(main);
      return found!;
    };
    // A string-typed property whose only value is written in an object literal, through a function call.
    const panelKey = (initializer('panelText') as import('typescript').CallExpression).arguments[0]!;
    assert.deepEqual(values.of(panelKey), { top: false, exact: ['ferry.harbour'], prefixes: [] });
    assert.deepEqual(values.of(initializer('fromOutside')), { top: false, exact: [], prefixes: ['ferry.'] }, 'what comes from outside is any string');
    // Per object (another object's property of the same name is not this one's), and a store's set per call (one
    // key's value is not another's), through the copy the store makes of what it is given.
    assert.deepEqual(values.of(initializer('storeKey')), { top: false, exact: ['ferry.dock', 'ferry.harbour'], prefixes: [] });
  });

  it('writes the shared code it uses as declarations: signatures with their documentation, and the types they name', () => {
    assert.ok(!b.uses.some((u) => u.unit === 'src/shared/math.ts#Range'), 'the feature does not name Range itself');
    const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'converter-interface-'));
    try {
      const written = writeInterface(ts, sp.checker, index, b, slices, dir);
      assert.deepEqual(written.map((w) => `${w.file} ${w.declarations}`), ['src/main.ts 10', 'src/only/snap.ts 1', 'src/shared/i18n.ts 1', 'src/shared/math.ts 5', 'src/shared/table.ts 1']);
      const math = fs.readFileSync(path.join(dir, 'src/shared/math.ts'), 'utf8');
      assert.match(math, /export function clamp\(v: number, lo: number, hi: number\): number;/);
      assert.match(math, /\/\*\* Whether a value is inside a span\. \*\/\nexport function within\(v: number, r: Range\): boolean;/);
      assert.match(math, /\/\*\* A span of values, from lo to hi\. \*\/\nexport interface Range \{\n {2}readonly lo: number;/, 'the type a signature names');
      assert.match(math, /export const FULL: Range = \{ lo: 0, hi: WIDE \};/);
      assert.match(math, /const WIDE = 100;/, 'the value another value is written from');
      assert.match(fs.readFileSync(path.join(dir, 'src/main.ts'), 'utf8'), /const PACE = 3;/, 'what a slice of shared code reads');
      assert.doesNotMatch(math, /return /, 'no body');
      assert.doesNotMatch(math, /roundTo/, 'the feature own code is not its interface');
    } finally {
      removeTree(dir);
    }
  });

  it('refuses a file that is not in the program', () => {
    assert.throws(() => featureBoundary(ir, ['src/nothing.ts'], roots, holds), /src\/nothing\.ts has no declarations/);
  });
});
