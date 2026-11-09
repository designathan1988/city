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

/**
 * The value of a numeric literal of TypeScript or C++: digit separators (1_000, 1'000) and type suffixes (0.5f, 10u,
 * 1ULL) removed; hex, binary and octal prefixes as written. NaN for anything else.
 */
export function literalValue(text: string): number {
  const plain = text.replace(/['_]/g, '');
  const sign = plain.startsWith('-') ? -1 : 1;
  const body = plain.replace(/^-/, '');
  if (/^0[xX]/.test(body)) return sign * Number(body.replace(/[uUlL]+$/, ''));
  if (/^0[bBoO]/.test(body)) return sign * Number(body.replace(/[uUlLn]+$/, ''));
  return sign * Number(body.replace(/(?<=[\d.])([fFlLuUn]+)$/, ''));
}

export function duplicatedConstants(ctx: AuditContext): RuleOutcome {
  if (ctx.options.allowedNumbers === null) return { skipped: 'needs --target-rules (its magic_numbers.allowed_values says which numbers may stand bare)' };
  const allowed = new Set(ctx.options.allowedNumbers);
  // Grouped by value: 0.5, .5f and 0.50 are one constant.
  const byText = new Map<string, Map<string, number[]>>();
  for (const unit of ctx.ir.units) {
    if (ctx.options.numberExempt.some((p) => unit.file.startsWith(p))) continue;
    for (const literal of unit.facts.numericLiterals) {
      const value = literalValue(literal.text);
      if (literal.context !== 'value' || Number.isNaN(value) || allowed.has(value)) continue;
      const key = String(value);
      const files = byText.get(key) ?? new Map<string, number[]>();
      files.set(unit.file, [...(files.get(unit.file) ?? []), literal.line]);
      byText.set(key, files);
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
