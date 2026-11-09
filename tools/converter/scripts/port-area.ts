// An area of the 4.0 as C++ for the game, written to a folder: `node scripts/port-area.ts <plan.json> <out>`. A plan names the
// module and its roots, and the other modules' roots translated with it, in one translation; the files go where the game's owners
// are. Prints what could not be written. The plan's `library` is a conversion library, read as `convert` reads it; its paths
// are the machine file's (`${game}/...`), and the game's C++ graph is derived from the game's code, never named by the plan.
import fs from 'node:fs';
import path from 'node:path';
import { targetGraph } from '../src/common/derived.ts';
import { writeTree } from '../src/common/files.ts';
import { machine } from '../src/common/machine.ts';
import { readLibrary } from '../src/convert/autospec.ts';
import { loadTypeScript } from '../src/graph/typescript.ts';
import { runObjectDifferential } from '../src/translate/differential.ts';
import { translate } from '../src/translate/run.ts';
import type { TranslationSpec } from '../src/translate/spec.ts';

interface AreaPlan { readonly source: string; readonly units: readonly string[]; readonly shared?: readonly string[]; readonly owner: string; readonly summary: string }

const [planFile, out] = process.argv.slice(2);
if (!planFile || !out) throw new Error('usage: node scripts/port-area.ts <plan.json> <out>');
const m = machine();
const source = m.get('source');
const written = JSON.parse(fs.readFileSync(planFile, 'utf8')) as AreaPlan & { library: string; targetRoot: string };
const plan = { ...written, library: m.expand(written.library), targetRoot: m.expand(written.targetRoot) };
const library = readLibrary(plan.library);
const cxx = m.get('cxx');
const spec: TranslationSpec = {
  feature: path.basename(planFile, '.json'), subsystem: plan.owner, namespace: library.namespace, source: plan.source, summary: plan.summary,
  state: { source: '', target: '', member: '', root: library.stateRoot }, units: plan.units, ...(plan.shared ? { shared: plan.shared } : {}), pending: [], number: library.number,
  types: { ...(library.types ?? {}) }, functions: library.functions, classes: library.classes, memoizers: library.memoizers, patterns: library.patterns, typeNames: {},
  layer: { rules: library.rules, includes: ['core/', `${plan.owner.replace(/^native\/src\//, '')}/`] },
} as unknown as TranslationSpec;
const typescriptDir = m.get('typescript');
const result = translate({ sourceRoot: source, tsconfig: path.join(source, 'tsconfig.json'), typescriptDir,
  spec, targetRoot: plan.targetRoot, targetGraph: targetGraph({ targetRoot: plan.targetRoot }) });
console.log(result.errors.length === 0 ? 'no errors' : result.errors.join('\n'));
if (result.dropped.length > 0) console.log(`left out: ${result.dropped.join('\n')}`);
writeTree(out, result.files);
console.log([...result.files.keys()].join('\n'));
// The light check: the area against the source, nothing of the game compiled but its headers.
if (result.errors.length === 0) {
  const tuning = JSON.parse(fs.readFileSync(path.join(plan.targetRoot, 'project/data/catalog/tuning.json'), 'utf8')) as Record<string, Record<string, number>>;
  const context = Object.fromEntries(Object.entries(tuning).flatMap(([section, rows]) => Object.entries(rows).filter(([, v]) => typeof v === 'number').map(([k, v]) => [`${section}_${k}`, v])));
  const started = Date.now();
  const check = runObjectDifferential({ ts: loadTypeScript(source, typescriptDir), tsconfig: path.join(source, 'tsconfig.json'), sourceRoot: source, translation: result,
    includes: [path.join(plan.targetRoot, 'native/src')], cxx, work: path.join(out, 'check'), steps: Number(process.env['STEPS'] ?? 3000), seed: 7, tolerance: 2e-5, valueTypes: result.valueTypes, context });
  console.log(`checked in ${Date.now() - started} ms: ${check.steps} calls, ${check.mismatches.length} mismatches; ${JSON.stringify(check.calls)}`);
  const byCall = new Map<string, number>();
  for (const m of check.mismatches) byCall.set(`${m.call.replace(/\(.*$/s, '')}${m.path.replace(/\.\d+/g, '.N')}`, (byCall.get(`${m.call.replace(/\(.*$/s, '')}${m.path.replace(/\.\d+/g, '.N')}`) ?? 0) + 1);
  if (byCall.size > 0) console.log(JSON.stringify([...byCall].sort((a, b) => b[1] - a[1]).slice(0, 12)));
  for (const m of check.mismatches.slice(0, 12)) console.log(`step ${m.step} ${m.call}${m.path}: source ${JSON.stringify(m.source)}, translation ${JSON.stringify(m.translation)}`);
  process.exitCode = check.mismatches.length > 0 ? 1 : 0;
}
