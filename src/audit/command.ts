// `converter audit`: runs the rule catalog over a graph IR (from `converter graph`) and writes JSON and Markdown.
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import type { GraphIR } from '../graph/ir.ts';
import { IR_VERSION } from '../graph/ir.ts';
import { PROFILE_VERSION, type Profile } from '../measure/profile.ts';
import type { AuditOptions } from './context.ts';
import { renderAudit } from './report.ts';
import { runAudit } from './run.ts';

export const AUDIT_FLAGS: readonly FlagSpec[] = [
  { name: 'graph', required: true, help: 'graph IR JSON written by `converter graph` (whole scope recommended)' },
  { name: 'out', required: true, help: 'JSON file to write the audit to' },
  { name: 'markdown', help: 'also write the readable report to this file' },
  { name: 'target-rules', help: 'the target project\'s gate rules JSON (max_lines, max_function_lines)' },
  { name: 'target-extension', help: 'which extension\'s limits apply to converted code (default: .cpp)' },
  { name: 'max-file-lines', help: 'file size limit (overrides --target-rules)' },
  { name: 'max-function-lines', help: 'function size limit (overrides --target-rules)' },
  { name: 'hot', repeatable: true, help: 'unit id, id prefix or file whose code runs every frame or tick' },
  { name: 'ui-dir', repeatable: true, help: 'directory of interface code, relative to the source root' },
  { name: 'state-dir', repeatable: true, help: 'directory of game state and simulation code, relative to the source root' },
  { name: 'sim-dir', repeatable: true, help: 'directory of simulation code that must be deterministic' },
  { name: 'feature', repeatable: true, help: 'name=unitId: a feature whose scattering is measured' },
  { name: 'max-scattering', help: 'degree of scattering at which a feature is reported (default 0.5, a project decision)' },
  { name: 'min-writer-files', help: 'files writing one state before it is reported as scattered (default 3)' },
  { name: 'min-constant-files', help: 'files repeating one value before it is reported (default 3)' },
  { name: 'profile', help: 'profile JSON from `converter measure`: performance severity from measured calls per frame' },
];

function readProfile(file: string | undefined, ir: GraphIR): Profile | null {
  if (file === undefined) return null;
  const profile = JSON.parse(fs.readFileSync(file, 'utf8')) as Profile;
  if (profile.version !== PROFILE_VERSION) throw new ArgsError(`${file} is profile version ${profile.version}; this audit reads ${PROFILE_VERSION}`);
  if (profile.sourceRoot !== ir.sourceRoot) throw new ArgsError(`${file} measured ${profile.sourceRoot}, the graph is of ${ir.sourceRoot}`);
  if (profile.graphUnits !== ir.units.length) throw new ArgsError(`${file} was measured against a graph of ${profile.graphUnits} units; this graph has ${ir.units.length}: measure again`);
  return profile;
}

function number(value: string | undefined, name: string): number | undefined {
  if (value === undefined) return undefined;
  const n = Number(value);
  if (!Number.isFinite(n) || n < 0) throw new ArgsError(`--${name} must be a non-negative number`);
  return n;
}

function targetLimits(file: string | undefined, extension: string): { file?: number; fn?: number } {
  if (file === undefined) return {};
  const rules = JSON.parse(fs.readFileSync(file, 'utf8')) as { max_lines?: Record<string, number>; max_function_lines?: Record<string, number> };
  const limits: { file?: number; fn?: number } = {};
  const fileLimit = rules.max_lines?.[extension];
  const fnLimit = rules.max_function_lines?.[extension];
  if (fileLimit === undefined || fnLimit === undefined) throw new ArgsError(`${file} has no max_lines / max_function_lines for ${extension}`);
  limits.file = fileLimit;
  limits.fn = fnLimit;
  return limits;
}

export function runAuditCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, AUDIT_FLAGS);
  const ir = JSON.parse(fs.readFileSync(flags.require('graph'), 'utf8')) as GraphIR;
  if (ir.version !== IR_VERSION) throw new ArgsError(`the graph is IR version ${ir.version}; this audit reads version ${IR_VERSION}: run \`converter graph\` again`);
  const limits = targetLimits(flags.one('target-rules'), flags.one('target-extension') ?? '.cpp');
  const options: AuditOptions = {
    hot: flags.all('hot'),
    uiDirs: flags.all('ui-dir'),
    stateDirs: flags.all('state-dir'),
    simDirs: flags.all('sim-dir'),
    maxFileLines: number(flags.one('max-file-lines'), 'max-file-lines') ?? limits.file,
    maxFunctionLines: number(flags.one('max-function-lines'), 'max-function-lines') ?? limits.fn,
    features: flags.all('feature').map((spec) => {
      const eq = spec.indexOf('=');
      if (eq <= 0) throw new ArgsError(`--feature ${spec}: expected name=unitId`);
      return { name: spec.slice(0, eq), entry: spec.slice(eq + 1) };
    }),
    maxScattering: number(flags.one('max-scattering'), 'max-scattering') ?? 0.5,
    minWriterFiles: number(flags.one('min-writer-files'), 'min-writer-files') ?? 3,
    minConstantFiles: number(flags.one('min-constant-files'), 'min-constant-files') ?? 3,
    profile: readProfile(flags.one('profile'), ir),
  };
  const result = runAudit(ir, options);
  const out = path.resolve(flags.require('out'));
  fs.mkdirSync(path.dirname(out), { recursive: true });
  fs.writeFileSync(out, JSON.stringify(result), 'utf8');
  const markdown = flags.one('markdown');
  if (markdown !== undefined) {
    fs.mkdirSync(path.dirname(path.resolve(markdown)), { recursive: true });
    fs.writeFileSync(markdown, renderAudit(result), 'utf8');
  }
  const counts = { high: 0, medium: 0, low: 0 };
  for (const f of result.findings) counts[f.severity]++;
  const skipped = result.rules.filter((r) => r.skipped).map((r) => r.id);
  console.log(`audit: ${result.findings.length} findings (${counts.high} high, ${counts.medium} medium, ${counts.low} low); ` +
    `hot path ${result.hot.units} units; ${skipped.length ? `not run: ${skipped.join(', ')}` : 'all rules run'} -> ${out}`);
}
