// `converter differential`: the translation of a feature run against its source on the same random scenario, in two
// builds. The logic build makes every number a double (JavaScript's own) and the target's value types doubles too:
// it must agree with the source but for the libraries' last bit. The target build is the translation as the game
// gets it (floats): it is compared within float precision. Everything happens in the output folder.
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import { removeTree } from '../common/files.ts';
import { normalizePath } from '../common/paths.ts';
import { loadTypeScript } from '../graph/typescript.ts';
import { runDifferential, type DifferentialResult } from './differential.ts';
import { LOGIC_HEADER, translate, type TranslationResult } from './run.ts';
import { readTranslationSpec } from './spec.ts';

export const DIFFERENTIAL_FLAGS: readonly FlagSpec[] = [
  { name: 'source-root', required: true, help: 'root of the TypeScript source project' },
  { name: 'tsconfig', help: 'the source\'s tsconfig.json (default: <source-root>/tsconfig.json)' },
  { name: 'typescript', help: 'TypeScript package directory (default: the source project\'s own)' },
  { name: 'spec', required: true, help: 'translation spec JSON' },
  { name: 'target-graph', required: true, help: 'the target\'s C++ graph JSON (converter cppgraph): the types it already has' },
  { name: 'target-root', required: true, help: 'root of the target game (read only: its headers and gate rules)' },
  { name: 'cxx', required: true, help: 'the C++ compiler driver' },
  { name: 'out', required: true, help: 'work folder (outside the target)' },
  { name: 'steps', help: 'calls per scenario (default 3000)' },
  { name: 'seeds', help: 'how many scenarios, seeds 1..n (default 3)' },
  { name: 'hand', repeatable: true, help: 'a C++ file written by hand, relative to the target root (native/src/...): it replaces the translation\'s file of that path, or is added; the scenario still calls the API the spec declares' },
];

/**
 * The translation with the files written by hand in place of the machine's: the scenario, the printer and the API
 * (the state type, the public functions with their C++ names, the structs) still come from the spec's translation,
 * so the hand-written code must declare that API in the same headers. A translation error is no longer fatal (the
 * function that does not translate is usually the one written by hand); the compile then decides.
 */
function withHandFiles(translation: TranslationResult, targetRoot: string, hand: readonly string[]): { translation: TranslationResult; notes: string[] } {
  if (hand.length === 0) return { translation, notes: [] };
  const files = new Map(translation.files);
  const notes: string[] = [];
  for (const rel of hand) {
    // the translation's own key form: a path relative to the target root, forward slashes
    const key = rel.split('\\').join('/').replace(/^\.\//, '');
    if (path.isAbsolute(key)) throw new ArgsError(`--hand ${rel}: give the path relative to the target root (native/src/...)`);
    const file = path.join(targetRoot, key);
    if (!fs.existsSync(file)) throw new ArgsError(`--hand ${rel}: no such file under the target root (${file})`);
    notes.push(`${files.has(key) ? 'replaced' : 'added'} by hand: ${key}`);
    files.set(key, fs.readFileSync(file, 'utf8'));
  }
  for (const e of translation.errors) notes.push(`translation error (the hand-written files stand in): ${e.split('\n')[0]}`);
  return { translation: { ...translation, files, errors: [] }, notes };
}

/** The logic build's value types: each target value type found, with double fields, under its Logic name. */
function writeLogicHeader(namespace: string, valueTypes: Readonly<Record<string, Readonly<Record<string, string>>>>, dir: string): void {
  const structs = Object.entries(valueTypes).map(([name, fields]) => {
    const names = Object.values(fields);
    return [`struct ${name} {`, `\tdouble ${names.map((n) => `${n} = 0`).join(', ')};`, `\t${name}() = default;`,
      `\t${name}(${names.map((n) => `double ${n}_`).join(', ')}) : ${names.map((n) => `${n}(${n}_)`).join(', ')} {}`, '};'].join('\n');
  });
  fs.mkdirSync(dir, { recursive: true });
  fs.writeFileSync(path.join(dir, LOGIC_HEADER), ['// The logic build\'s value types: double fields.', '#pragma once', '', `namespace ${namespace} {`, '',
    structs.join('\n\n'), '', `} // namespace ${namespace}`, ''].join('\n'));
}

function report(name: string, results: DifferentialResult[]): string[] {
  const mismatches = results.flatMap((r) => r.mismatches);
  const lines = [`${name}: ${results.length} scenario(s), ${results.reduce((n, r) => n + r.steps, 0)} calls, ${mismatches.length} difference(s)`];
  const calls: Record<string, number> = {};
  for (const r of results) for (const [k, v] of Object.entries(r.calls)) calls[k] = (calls[k] ?? 0) + v;
  lines.push(`  calls: ${Object.entries(calls).map(([k, v]) => `${k} ${v}`).join(', ')}`);
  for (const m of mismatches.slice(0, 15)) lines.push(`  step ${m.step} ${m.call.slice(0, 120)}\n    at ${m.path}: source ${JSON.stringify(m.source)} translation ${JSON.stringify(m.translation)}`);
  return lines;
}

export function runDifferentialCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, DIFFERENTIAL_FLAGS);
  const sourceRoot = path.resolve(flags.require('source-root'));
  const targetRoot = path.resolve(flags.require('target-root'));
  const out = path.resolve(flags.require('out'));
  if (normalizePath(out).startsWith(normalizePath(targetRoot) + '/')) throw new ArgsError(`--out must be outside the target (${out})`);
  // A clean work folder: nothing left by an earlier run may stand in for what this run makes.
  removeTree(out);
  const tsconfig = path.resolve(flags.one('tsconfig') ?? path.join(sourceRoot, 'tsconfig.json'));
  const spec = readTranslationSpec(flags.require('spec'));
  if (!spec.state.empty) throw new ArgsError('the spec names no state.empty: the scenario needs the empty state');
  const steps = Number(flags.one('steps') ?? 3000);
  const seeds = Number(flags.one('seeds') ?? 3);
  const typescriptDir = flags.one('typescript');
  const targetGraph = path.resolve(flags.require('target-graph'));
  const ts = loadTypeScript(sourceRoot, typescriptDir);
  const hand = flags.all('hand');
  // With files written by hand only the target build runs: hand-written code has the game's own number types, which
  // the logic build's doubles cannot reach
  const builds = [
    ...(hand.length ? [] : [{ name: 'logic (doubles)', spec: { ...spec, number: 'double' }, logic: true, tolerance: 1e-9, includes: [path.join(out, 'logic-headers')] }]),
    { name: hand.length ? 'target (floats), files written by hand' : 'target (floats)', spec, logic: false, tolerance: 1e-4, includes: [] as string[] },
  ];
  const lines: string[] = [];
  let failed = false;
  for (const build of builds) {
    const made = translate({ sourceRoot, tsconfig, typescriptDir, spec: build.spec, targetRoot, targetGraph, logic: build.logic });
    const { translation, notes } = withHandFiles(made, targetRoot, hand);
    lines.push(...notes.map((n) => `  ${n}`));
    const valueTypes = translation.valueTypes;
    if (build.logic) writeLogicHeader(spec.namespace, valueTypes, path.join(out, 'logic-headers'));
    if (translation.errors.length > 0) throw new Error(`the translation has errors:\n${translation.errors.join('\n')}`);
    const results: DifferentialResult[] = [];
    for (let seed = 1; seed <= seeds; seed++) {
      results.push(runDifferential({ ts, tsconfig, sourceRoot, empty: spec.state.empty, translation,
        includes: [...build.includes, path.join(targetRoot, 'native', 'src')], cxx: flags.require('cxx'),
        work: path.join(out, `${build.spec.number}-seed${seed}`), steps, seed, tolerance: build.tolerance, valueTypes }));
    }
    lines.push(...report(build.name, results));
    if (results.some((r) => r.mismatches.length > 0)) failed = true;
  }
  fs.writeFileSync(path.join(out, 'differential.txt'), lines.join('\n') + '\n');
  console.log(lines.join('\n'));
  process.exitCode = failed ? 1 : 0;
}
