// `converter audit`: runs the rule catalog over a graph IR (from `converter graph`) and writes JSON and Markdown.
import fs from 'node:fs';
import path from 'node:path';
import { ArgsError, parseFlags, type FlagSpec } from '../common/args.ts';
import type { GraphIR } from '../graph/ir.ts';
import { IR_VERSION } from '../graph/ir.ts';
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
  { name: 'writer-dir', repeatable: true, help: 'folder the architecture designates to change state owned elsewhere (commands, time step, loading)' },
  { name: 'channel', repeatable: true, help: 'unit id (or prefix) of a designated shared channel every producer writes (a change log)' },
  { name: 'state-root', help: 'unit id of the class holding all state (the state manager): its members are owned by the subsystem of their type' },
  { name: 'min-constant-files', help: 'files repeating one value before it is reported (default 3)' },
];

function number(value: string | undefined, name: string): number | undefined {
  if (value === undefined) return undefined;
  const n = Number(value);
  if (!Number.isFinite(n) || n < 0) throw new ArgsError(`--${name} must be a non-negative number`);
  return n;
}

/** Degree of scattering at which a feature is reported: a project decision (Eaddy et al. give no threshold). */
const DEFAULT_MAX_SCATTERING = 0.5;
const DEFAULT_MIN_CONSTANT_FILES = 3;

interface TargetRules {
  readonly file?: number;
  readonly fn?: number;
  readonly allowed: readonly number[] | null;
  readonly exempt: readonly string[];
}

/** What the target project's gate rules (rules.json) say about sizes and bare numbers. */
function targetRules(file: string | undefined, extension: string): TargetRules {
  if (file === undefined) return { allowed: null, exempt: [] };
  const rules = JSON.parse(fs.readFileSync(file, 'utf8')) as {
    max_lines?: Record<string, number>; max_function_lines?: Record<string, number>; magic_numbers?: { allowed_values?: number[]; exempt?: string[] };
  };
  const fileLimit = rules.max_lines?.[extension];
  const fnLimit = rules.max_function_lines?.[extension];
  const allowed = rules.magic_numbers?.allowed_values;
  if (fileLimit === undefined || fnLimit === undefined) throw new ArgsError(`${file} has no max_lines / max_function_lines for ${extension}`);
  if (!Array.isArray(allowed)) throw new ArgsError(`${file} has no magic_numbers.allowed_values`);
  return { file: fileLimit, fn: fnLimit, allowed, exempt: rules.magic_numbers?.exempt ?? [] };
}

export function runAuditCommand(argv: readonly string[]): void {
  const flags = parseFlags(argv, AUDIT_FLAGS);
  const ir = JSON.parse(fs.readFileSync(flags.require('graph'), 'utf8')) as GraphIR;
  if (ir.version !== IR_VERSION) throw new ArgsError(`the graph is IR version ${ir.version}; this audit reads version ${IR_VERSION}: run \`converter graph\` again`);
  const limits = targetRules(flags.one('target-rules'), flags.one('target-extension') ?? '.cpp');
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
    maxScattering: number(flags.one('max-scattering'), 'max-scattering') ?? DEFAULT_MAX_SCATTERING,
    writerDirs: flags.all('writer-dir'),
    channels: flags.all('channel'),
    stateRoot: flags.one('state-root') ?? null,
    minConstantFiles: number(flags.one('min-constant-files'), 'min-constant-files') ?? DEFAULT_MIN_CONSTANT_FILES,
    allowedNumbers: limits.allowed,
    numberExempt: limits.exempt,
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
