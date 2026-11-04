// Performance rules: methodology patterns recorded by the graph, allocations in code that runs every frame, one mesh
// per item, and GPU resources never disposed. Severity comes from the measured profile when there is one (calls per
// frame in a real run, per quality tier): at least once per frame is high, less is medium, not executed is low.
// Without a profile, it falls back to static reachability from the repeating loops, which is every path a loop *can*
// take: rated medium at most, and said so.
import type { PatternSite } from '../graph/ir.ts';
import type { AuditContext, Finding, RuleOutcome, Severity } from './context.ts';

interface Rating {
  readonly severity: Severity;
  readonly note: string;
  readonly runs: boolean;
}

function format(perFrame: number): string {
  return perFrame >= 10 ? perFrame.toFixed(0) : perFrame >= 1 ? perFrame.toFixed(1) : perFrame.toPrecision(2);
}

function rate(ctx: AuditContext, unitId: string, unmeasuredCold: Severity): Rating {
  if (ctx.measured) {
    const m = ctx.measured.get(unitId);
    if (!m) return { severity: 'low', note: ' (not executed in the measured run)', runs: false };
    const note = ` (measured ${format(m.perFrame)} calls per frame, ${m.variant})`;
    return { severity: m.perFrame >= 1 ? 'high' : 'medium', note, runs: true };
  }
  return ctx.hot.has(unitId)
    ? { severity: 'medium', note: ' (reachable from a frame or tick loop; not measured)', runs: true }
    : { severity: unmeasuredCold, note: '', runs: false };
}

export function patternRule(ctx: AuditContext, id: PatternSite['id'], coldSeverity: Severity = 'low'): RuleOutcome {
  const findings: Finding[] = [];
  for (const unit of ctx.ir.units) {
    for (const site of unit.facts.patterns) {
      if (site.id !== id) continue;
      const rating = rate(ctx, unit.id, coldSeverity);
      findings.push({ rule: id, severity: rating.severity, unit: unit.id, file: unit.file, line: site.line,
        message: `${unit.name}${rating.note}: ${site.detail}` });
    }
  }
  return findings;
}

export function hotAllocation(ctx: AuditContext): RuleOutcome {
  if (!ctx.measured && ctx.hot.size === 0) return { skipped: 'no profile (--profile) and no repeating loop or --hot root found' };
  const findings: Finding[] = [];
  for (const unit of ctx.ir.units) {
    const sites = unit.facts.loopAllocations;
    if (sites.length === 0) continue;
    const rating = rate(ctx, unit.id, 'low');
    if (!rating.runs) continue;
    const counts = new Map<string, number>();
    for (const site of sites) counts.set(site.what, (counts.get(site.what) ?? 0) + 1);
    const top = [...counts].sort((a, b) => b[1] - a[1]).slice(0, 4).map(([what, n]) => `${n}× ${what}`).join(', ');
    findings.push({ rule: 'hot-allocation', severity: rating.severity, unit: unit.id, file: unit.file, line: sites[0]!.line,
      message: `${unit.name}${rating.note}: ${sites.length} allocation site(s) inside loops (${top})` });
  }
  return findings;
}

const PER_OBJECT_MESH = /^new (THREE\.)?(Mesh|SkinnedMesh|Line|LineSegments|LineLoop|Points|Sprite)\b/;

export function perObjectDraw(ctx: AuditContext): RuleOutcome {
  const findings: Finding[] = [];
  for (const unit of ctx.ir.units) {
    for (const site of unit.facts.loopAllocations) {
      if (!PER_OBJECT_MESH.test(site.what)) continue;
      const rating = rate(ctx, unit.id, 'medium');
      findings.push({ rule: 'per-object-draw', severity: rating.severity === 'high' ? 'high' : rating.runs ? 'medium' : rating.severity,
        unit: unit.id, file: unit.file, line: site.line, message: `${unit.name}${rating.note}: ${site.what} inside a loop` });
    }
  }
  return findings;
}

const GPU_RESOURCE = /(Geometry|Material|Texture|RenderTarget)$/;

export function undisposedGpuResources(ctx: AuditContext): RuleOutcome {
  const created = new Map<string, { types: Set<string>; line: number; unit: string }>();
  const disposes = new Set<string>();
  for (const unit of ctx.ir.units) {
    for (const use of unit.facts.external) {
      if (!use.key.startsWith('three:')) continue;
      const symbol = use.key.slice('three:'.length);
      if (use.kind === 'call' && symbol.endsWith('.dispose')) disposes.add(unit.file);
      if (use.kind === 'construct' && GPU_RESOURCE.test(symbol)) {
        const entry = created.get(unit.file) ?? { types: new Set<string>(), line: unit.start.line, unit: unit.id };
        entry.types.add(symbol);
        created.set(unit.file, entry);
      }
    }
  }
  return [...created]
    .filter(([file]) => !disposes.has(file))
    .map(([file, entry]): Finding => ({ rule: 'undisposed-gpu-resource', severity: 'medium', unit: entry.unit, file, line: entry.line,
      message: `creates ${[...entry.types].sort().join(', ')} and never calls dispose() in this file` }));
}
