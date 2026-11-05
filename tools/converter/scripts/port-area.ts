// An area of the 4.0 as C++ for the game, written to a folder: `node scripts/port-area.ts <plan.json> <out>`. A plan names the
// module and its roots, and the other modules' roots translated with it, in one translation; the files go where the game's owners
// are. Prints what could not be written. The plan's `library` is a conversion library, read as `convert` reads it.
import fs from 'node:fs';
import path from 'node:path';
import { writeTree } from '../src/common/files.ts';
import { readLibrary } from '../src/convert/autospec.ts';
import { translate } from '../src/translate/run.ts';
import type { TranslationSpec } from '../src/translate/spec.ts';

interface AreaPlan { readonly source: string; readonly units: readonly string[]; readonly shared?: readonly string[]; readonly owner: string; readonly summary: string }

const [planFile, out] = process.argv.slice(2);
if (!planFile || !out) throw new Error('usage: node scripts/port-area.ts <plan.json> <out>');
const here = path.dirname(new URL(import.meta.url).pathname.replace(/^\/([A-Za-z]:)/, '$1'));
const machine = JSON.parse(fs.readFileSync(path.join(here, '..', 'configs', 'roadcraft-machine.json'), 'utf8')) as { source: string };
const plan = JSON.parse(fs.readFileSync(planFile, 'utf8')) as AreaPlan & { library: string; targetRoot: string; targetGraph: string };
const library = readLibrary(plan.library);
const spec: TranslationSpec = {
  feature: path.basename(planFile, '.json'), subsystem: plan.owner, namespace: library.namespace, source: plan.source, summary: plan.summary,
  state: { source: '', target: '', member: '', root: library.stateRoot }, units: plan.units, ...(plan.shared ? { shared: plan.shared } : {}), pending: [], number: library.number,
  types: { ...(library.types ?? {}) }, functions: library.functions, classes: library.classes, memoizers: library.memoizers, patterns: library.patterns, typeNames: {},
  layer: { rules: library.rules, includes: ['core/', `${plan.owner.replace(/^native\/src\//, '')}/`] },
} as unknown as TranslationSpec;
const result = translate({ sourceRoot: machine.source, tsconfig: path.join(machine.source, 'tsconfig.json'), typescriptDir: path.join(machine.source, 'node_modules', 'typescript'),
  spec, targetRoot: plan.targetRoot, targetGraph: plan.targetGraph });
console.log(result.errors.length === 0 ? 'no errors' : result.errors.join('\n'));
if (result.dropped.length > 0) console.log(`left out: ${result.dropped.join('\n')}`);
writeTree(out, result.files);
console.log([...result.files.keys()].join('\n'));
