// Architecture rules: god files and functions (against the target's limits), duplicated code (shared winnowing
// fingerprints), duplicated constants, and feature scattering (Eaddy's degree of scattering over files).
import type { EdgeKind } from '../graph/ir.ts';
import type { AuditContext, Finding, RuleOutcome } from './context.ts';

export function godFiles(ctx: AuditContext): RuleOutcome {
  const max = ctx.options.maxFileLines;
  if (max === undefined) return { skipped: 'needs --target-rules or --max-file-lines' };
  return ctx.ir.files
    .filter((f) => f.lines > max)
    .map((f): Finding => ({ rule: 'god-file', severity: f.lines > 2 * max ? 'high' : 'medium', unit: null, file: f.path, line: 1,
      message: `${f.lines} lines (limit ${max})` }));
}

export function godFunctions(ctx: AuditContext): RuleOutcome {
  const max = ctx.options.maxFunctionLines;
  if (max === undefined) return { skipped: 'needs --target-rules or --max-function-lines' };
  const findings: Finding[] = [];
  for (const unit of ctx.ir.units) {
    if (!unit.facts.callable) continue;
    const own = ctx.ownLines(unit);
    if (own <= max) continue;
    findings.push({ rule: 'god-function', severity: own > 2 * max ? 'high' : 'medium', unit: unit.id, file: unit.file,
      line: unit.start.line, message: `${unit.name}: ${own} lines of its own (limit ${max})` });
  }
  return findings;
}

export function duplicatedCode(ctx: AuditContext): RuleOutcome {
  const byHash = new Map<number, { id: string; line: number }[]>();
  for (const unit of ctx.ir.units) {
    for (const [hash, line] of unit.facts.fingerprints) {
      const list = byHash.get(hash) ?? [];
      if (!list.some((e) => e.id === unit.id)) list.push({ id: unit.id, line });
      byHash.set(hash, list);
    }
  }
  const pairs = new Map<string, { a: { id: string; line: number }; b: { id: string; line: number }; shared: number }>();
  for (const sites of byHash.values()) {
    if (sites.length < 2) continue;
    for (let i = 0; i < sites.length; i++) {
      for (let j = i + 1; j < sites.length; j++) {
        const [a, b] = sites[i]!.id < sites[j]!.id ? [sites[i]!, sites[j]!] : [sites[j]!, sites[i]!];
        const key = `${a.id}\n${b.id}`;
        const pair = pairs.get(key);
        if (pair) pair.shared++;
        else pairs.set(key, { a, b, shared: 1 });
      }
    }
  }
  return [...pairs.values()].map(({ a, b, shared }): Finding => {
    const unit = ctx.byId.get(a.id)!;
    const other = ctx.byId.get(b.id)!;
    return { rule: 'duplicated-code', severity: shared >= 3 ? 'medium' : 'low', unit: a.id, file: unit.file, line: a.line,
      message: `${unit.name} and ${other.name} (${other.file}:${b.line}) share ${shared} fingerprint(s), each an identical 40-token run` };
  });
}

export function duplicatedConstants(ctx: AuditContext): RuleOutcome {
  const byText = new Map<string, Map<string, number[]>>();
  for (const unit of ctx.ir.units) {
    for (const literal of unit.facts.numericLiterals) {
      if (literal.trivial || literal.context !== 'value') continue;
      const files = byText.get(literal.text) ?? new Map<string, number[]>();
      files.set(unit.file, [...(files.get(unit.file) ?? []), literal.line]);
      byText.set(literal.text, files);
    }
  }
  const findings: Finding[] = [];
  for (const [text, files] of byText) {
    if (files.size < ctx.options.minConstantFiles) continue;
    const sorted = [...files].sort(([a], [b]) => a.localeCompare(b));
    const [file, lines] = sorted[0]!;
    const uses = sorted.reduce((n, [, l]) => n + l.length, 0);
    findings.push({ rule: 'duplicated-constant', severity: 'low', unit: null, file, line: lines[0]!,
      message: `${text} appears ${uses} times in ${files.size} files (${sorted.slice(0, 4).map(([f]) => f).join(', ')}${files.size > 4 ? ', ...' : ''})` });
  }
  return findings;
}

const FEATURE_EDGES = new Set<EdgeKind>(['call', 'construct', 'read', 'write', 'reference', 'implementation', 'callbackAssign', 'moduleLoad']);

export interface FeatureMetrics {
  readonly name: string;
  readonly entry: string;
  readonly units: number;
  readonly files: number;
  readonly degreeOfScattering: number;
}

/** CONC(s,t) = own lines of feature s in file t / own lines of s; DOS = 1 − |T|·Σ(CONC − 1/|T|)² / (|T| − 1). */
export function featureMetrics(ctx: AuditContext): FeatureMetrics[] {
  const total = ctx.ir.files.length;
  return ctx.options.features.map(({ name, entry }) => {
    if (!ctx.byId.has(entry)) throw new Error(`--feature ${name}=${entry}: no unit with that id in the graph`);
    const closure = ctx.reach([entry], (edge) => FEATURE_EDGES.has(edge.kind));
    const perFile = new Map<string, number>();
    let lines = 0;
    for (const id of closure) {
      const unit = ctx.byId.get(id)!;
      const own = Math.max(0, ctx.ownLines(unit));
      perFile.set(unit.file, (perFile.get(unit.file) ?? 0) + own);
      lines += own;
    }
    let sum = 0;
    for (const file of ctx.ir.files) {
      const conc = lines > 0 ? (perFile.get(file.path) ?? 0) / lines : 0;
      sum += (conc - 1 / total) ** 2;
    }
    const dos = total > 1 ? 1 - (total * sum) / (total - 1) : 0;
    return { name, entry, units: closure.size, files: perFile.size, degreeOfScattering: Math.round(dos * 1000) / 1000 };
  });
}

export function featureScattering(ctx: AuditContext, metrics: readonly FeatureMetrics[]): RuleOutcome {
  if (ctx.options.features.length === 0) return { skipped: 'needs --feature name=unitId' };
  return metrics
    .filter((m) => m.degreeOfScattering >= ctx.options.maxScattering)
    .map((m): Finding => {
      const unit = ctx.byId.get(m.entry)!;
      return { rule: 'feature-scattering', severity: 'medium', unit: m.entry, file: unit.file, line: unit.start.line,
        message: `${m.name}: ${m.units} units in ${m.files} files, degree of scattering ${m.degreeOfScattering} (reported at ≥ ${ctx.options.maxScattering})` };
    });
}
