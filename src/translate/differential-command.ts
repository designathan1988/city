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
import { translate } from './run.ts';
import { readTranslationSpec, type TranslationSpec } from './spec.ts';

export const DIFFERENTIAL_FLAGS: readonly FlagSpec[] = [
  { name: 'source-root', required: true, help: 'root of the TypeScript source project' },
  { name: 'tsconfig', help: 'the source\'s tsconfig.json (default: <source-root>/tsconfig.json)' },
  { name: 'typescript', help: 'TypeScript package directory (default: the source project\'s own)' },
  { name: 'spec', required: true, help: 'translation spec JSON' },
  { name: 'target-root', required: true, help: 'root of the target game (read only: its headers and gate rules)' },
  { name: 'cxx', required: true, help: 'the C++ compiler driver' },
  { name: 'out', required: true, help: 'work folder (outside the target)' },
  { name: 'steps', help: 'calls per scenario (default 3000)' },
  { name: 'seeds', help: 'how many scenarios, seeds 1..n (default 3)' },
];

/** The logic build's spec: every number a double, and each value type of the target a type of its own with double
 * fields (the target's headers stay as they are: a header standing in for one of them would lose what it includes). */
const LOGIC_HEADER = 'converter_logic_values.h';

function logicSpec(spec: TranslationSpec, dir: string): TranslationSpec {
  const types: Record<string, TranslationSpec['types'][string]> = {};
  const structs: string[] = [];
  for (const [source, t] of Object.entries(spec.types)) {
    if (!t.fields) {
      types[source] = t;
      continue;
    }
    const name = `Logic${t.type}`;
    const names = Object.keys(t.fields);
    structs.push([
      `struct ${name} {`,
      `\tdouble ${names.map((n) => `${n} = 0`).join(', ')};`,
      `\t${name}() = default;`,
      `\t${name}(${names.map((n) => `double ${n}_`).join(', ')}) : ${names.map((n) => `${n}(${n}_)`).join(', ')} {}`,
      '};',
    ].join('\n'));
    types[source] = { type: name, include: LOGIC_HEADER, fields: t.fields };
  }
  fs.mkdirSync(dir, { recursive: true });
  const header = ['// The logic build\'s value types: double fields.', '#pragma once', '', `namespace ${spec.namespace} {`, '',
    structs.join('\n\n'), '', `} // namespace ${spec.namespace}`, ''];
  fs.writeFileSync(path.join(dir, LOGIC_HEADER), header.join('\n'));
  return { ...spec, number: 'double', types };
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
  const ts = loadTypeScript(sourceRoot, typescriptDir);
  const builds = [
    { name: 'logic (doubles)', spec: logicSpec(spec, path.join(out, 'logic-headers')), tolerance: 1e-9, includes: [path.join(out, 'logic-headers')] },
    { name: 'target (floats)', spec, tolerance: 1e-4, includes: [] as string[] },
  ];
  const lines: string[] = [];
  let failed = false;
  for (const build of builds) {
    const valueTypes = Object.fromEntries(Object.values(build.spec.types).filter((t) => t.fields).map((t) => [t.type, t.fields!]));
    const translation = translate({ sourceRoot, tsconfig, typescriptDir, spec: build.spec, targetRoot });
    if (translation.errors.length > 0) throw new Error(`the translation has errors:\n${translation.errors.join('\n')}`);
    const results: DifferentialResult[] = [];
    for (let seed = 1; seed <= seeds; seed++) {
      results.push(runDifferential({ ts, tsconfig, sourceFile: path.join(sourceRoot, spec.source), empty: spec.state.empty, translation,
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
