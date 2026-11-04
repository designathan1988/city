// Performance rules: methodology patterns recorded by the graph, allocations on the frame or tick path, one mesh per
// item, and GPU resources never disposed. Severity follows the hot set, which is static reachability from the
// repeating loops: every path the loop *can* take, not what it does every frame. Reachable code is therefore rated
// medium, never high; "runs every frame" is a measurement, which the trace stage provides.
import type { PatternSite } from '../graph/ir.ts';
import type { AuditContext, Finding, RuleOutcome, Severity } from './context.ts';

/** Static reachability from a loop: possibly every frame, not measured. */
const REACHABLE: Severity = 'medium';
const REACHABLE_NOTE = ' (reachable from a frame or tick loop; not measured)';

export function patternRule(ctx: AuditContext, id: PatternSite['id'], coldSeverity: Severity = 'low'): RuleOutcome {
  const findings: Finding[] = [];
  for (const unit of ctx.ir.units) {
    for (const site of unit.facts.patterns) {
      if (site.id !== id) continue;
      const reachable = ctx.hot.has(unit.id);
      findings.push({ rule: id, severity: reachable ? REACHABLE : coldSeverity, unit: unit.id, file: unit.file, line: site.line,
        message: `${unit.name}${reachable ? REACHABLE_NOTE : ''}: ${site.detail}` });
    }
  }
  return findings;
}

export function hotAllocation(ctx: AuditContext): RuleOutcome {
  if (ctx.hot.size === 0) return { skipped: 'no hot roots: no requestAnimationFrame use found and no --hot given' };
  const findings: Finding[] = [];
  for (const id of ctx.hot) {
    const unit = ctx.byId.get(id)!;
    const sites = unit.facts.loopAllocations;
    if (sites.length === 0) continue;
    const counts = new Map<string, number>();
    for (const site of sites) counts.set(site.what, (counts.get(site.what) ?? 0) + 1);
    const top = [...counts].sort((a, b) => b[1] - a[1]).slice(0, 4).map(([what, n]) => `${n}× ${what}`).join(', ');
    findings.push({ rule: 'hot-allocation', severity: REACHABLE, unit: unit.id, file: unit.file, line: sites[0]!.line,
      message: `${unit.name}${REACHABLE_NOTE}: ${sites.length} allocation(s) inside loops (${top})` });
  }
  return findings;
}

const PER_OBJECT_MESH = /^new (THREE\.)?(Mesh|SkinnedMesh|Line|LineSegments|LineLoop|Points|Sprite)\b/;

export function perObjectDraw(ctx: AuditContext): RuleOutcome {
  const findings: Finding[] = [];
  for (const unit of ctx.ir.units) {
    for (const site of unit.facts.loopAllocations) {
      if (!PER_OBJECT_MESH.test(site.what)) continue;
      const reachable = ctx.hot.has(unit.id);
      findings.push({ rule: 'per-object-draw', severity: 'medium', unit: unit.id, file: unit.file,
        line: site.line, message: `${unit.name}${reachable ? REACHABLE_NOTE : ''}: ${site.what} inside a loop` });
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
