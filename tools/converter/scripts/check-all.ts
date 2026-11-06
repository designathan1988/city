// Every translated module against the 4.0, with no game: `node scripts/check-all.ts <ledger.json> <plan.json> <out> <shard> <shards>`.
// Each module of the ledger's written units is translated alone (the program opened once per shard), its classes and exported functions
// called at random in the source and in C++, each answer compared. One line of JSON per module in <out>/shard-<n>.jsonl. A translation
// that compiles but answers differently is found here, which no compiler sees.
import fs from 'node:fs';
import path from 'node:path';
import { removeTree } from '../src/common/files.ts';
import { readLibrary } from '../src/convert/autospec.ts';
import { loadTypeScript } from '../src/graph/typescript.ts';
import { runObjectDifferential } from '../src/translate/differential.ts';
import { prepareTranslation, translate } from '../src/translate/run.ts';
import type { TranslationSpec } from '../src/translate/spec.ts';

const [ledgerFile, planFile, out, shardText, shardsText] = process.argv.slice(2);
if (!ledgerFile || !planFile || !out || !shardText || !shardsText) throw new Error('usage: node scripts/check-all.ts <ledger.json> <plan.json> <out> <shard> <shards>');
const here = path.dirname(new URL(import.meta.url).pathname.replace(/^\/([A-Za-z]:)/, '$1'));
const machine = JSON.parse(fs.readFileSync(path.join(here, '..', 'configs', 'roadcraft-machine.json'), 'utf8')) as { source: string };
const plan = JSON.parse(fs.readFileSync(planFile, 'utf8')) as { library: string; targetRoot: string; targetGraph: string };
const ledger = JSON.parse(fs.readFileSync(ledgerFile, 'utf8')) as { done: string[]; units: { id: string; kind: string; file: string }[] };
const library = readLibrary(plan.library);
const cxx = process.env['CXX']!;
const shard = Number(shardText);
const shards = Number(shardsText);
const done = new Set(ledger.done);
const roots = new Map<string, string[]>();
for (const u of ledger.units) {
  if (!done.has(u.id) || (u.kind !== 'function' && u.kind !== 'class')) continue;
  const name = u.id.slice(u.id.indexOf('#') + 1);
  if (name.includes('.')) continue;
  roots.set(u.file, [...(roots.get(u.file) ?? []), name]);
}
const modules = [...roots.keys()].sort().filter((_, i) => i % shards === shard);
const specOf = (source: string, units: readonly string[]): TranslationSpec => ({
  feature: 'check', subsystem: 'native/src/sim/check', namespace: library.namespace, source, summary: source,
  state: { source: '', target: '', member: '', root: library.stateRoot }, units, pending: [], number: library.number, types: { ...(library.types ?? {}) }, functions: library.functions,
  classes: library.classes, memoizers: library.memoizers, patterns: library.patterns, typeNames: {}, layer: { rules: library.rules, includes: ['core/'] } } as unknown as TranslationSpec);
const base = { sourceRoot: machine.source, tsconfig: path.join(machine.source, 'tsconfig.json'), typescriptDir: path.join(machine.source, 'node_modules', 'typescript'), targetRoot: plan.targetRoot, targetGraph: plan.targetGraph };
const prepared = prepareTranslation({ ...base, spec: specOf(modules[0]!, roots.get(modules[0]!)!) });
const tuning = JSON.parse(fs.readFileSync(path.join(plan.targetRoot, 'project/data/catalog/tuning.json'), 'utf8')) as Record<string, Record<string, number>>;
const context = Object.fromEntries(Object.entries(tuning).flatMap(([section, rows]) => Object.entries(rows).filter(([, v]) => typeof v === 'number').map(([k, v]) => [`${section}_${k}`, v])));
const ts = loadTypeScript(machine.source, base.typescriptDir);
fs.mkdirSync(out, { recursive: true });
const log = path.join(out, `shard-${shard}.jsonl`);
fs.writeFileSync(log, '');
for (const module of modules) {
  const started = Date.now();
  const work = path.join(out, `work-${shard}`);
  let row: Record<string, unknown>;
  try {
    const translation = translate({ ...base, spec: specOf(module, roots.get(module)!), prepared });
    if (translation.errors.length > 0) row = { state: 'not-translated', why: translation.errors[0] };
    else {
      const check = runObjectDifferential({ ts, tsconfig: base.tsconfig, sourceRoot: machine.source, translation, includes: [path.join(plan.targetRoot, 'native/src')], cxx, work, steps: 400, seed: 7,
        tolerance: 1e-4, valueTypes: translation.valueTypes, context, hooksFile: path.join(here, 'hooks40.mjs') });
      row = { state: check.mismatches.length === 0 ? 'same' : 'different', calls: Object.keys(check.calls).length, mismatches: check.mismatches.length, first: check.mismatches[0] };
    }
  } catch (error) {
    const failed = error as Error & { stderr?: unknown };
    const text = `${failed.message ?? error}\n${String(failed.stderr ?? '').split('\n').filter((l) => /Error|error/.test(l)).slice(0, 3).join(' | ')}`;
    row = { state: /nothing of this translation can be called/.test(text) ? 'nothing-to-call' : /does not compile/.test(text) ? 'harness-does-not-compile' : 'source-failed', why: text.replace(/Command failed:[^\n]*\n/, '').slice(0, 300) };
  }
  removeTree(work);
  fs.appendFileSync(log, `${JSON.stringify({ module, ms: Date.now() - started, ...row })}\n`);
}
