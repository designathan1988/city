// State rules: module-level mutable state, state written from many files, interface code writing game state, and
// simulation code that depends on the wall clock or unseeded randomness.
import type { AuditContext, Finding, RuleOutcome } from './context.ts';

function writerFiles(ctx: AuditContext, target: string): string[] {
  const files = new Set<string>();
  for (const edge of ctx.incoming.get(target) ?? []) {
    if (edge.kind === 'write') files.add(ctx.byId.get(edge.from)!.file);
  }
  return [...files].sort();
}

export function moduleState(ctx: AuditContext): RuleOutcome {
  return ctx.ir.units
    .filter((u) => u.facts.mutableModuleState)
    .map((u): Finding => {
      const files = writerFiles(ctx, u.id);
      return { rule: 'module-state', severity: files.length > 1 ? 'high' : 'medium', unit: u.id, file: u.file, line: u.start.line,
        message: `${u.name}: mutable module variable written from ${files.length} file(s)${files.length ? ` (${files.slice(0, 4).join(', ')})` : ''}` };
    });
}

export function scatteredWrites(ctx: AuditContext): RuleOutcome {
  const findings: Finding[] = [];
  for (const unit of ctx.ir.units) {
    const files = writerFiles(ctx, unit.id);
    if (files.length < ctx.options.minWriterFiles) continue;
    findings.push({ rule: 'scattered-writes', severity: files.length >= 2 * ctx.options.minWriterFiles ? 'high' : 'medium',
      unit: unit.id, file: unit.file, line: unit.start.line,
      message: `${unit.name} is written from ${files.length} files: ${files.slice(0, 6).join(', ')}${files.length > 6 ? ', ...' : ''}` });
  }
  return findings;
}

export function uiWritesState(ctx: AuditContext): RuleOutcome {
  const { uiDirs, stateDirs } = ctx.options;
  if (uiDirs.length === 0 || stateDirs.length === 0) return { skipped: 'needs --ui-dir and --state-dir' };
  const findings: Finding[] = [];
  for (const edge of ctx.ir.edges) {
    if (edge.kind !== 'write') continue;
    const from = ctx.byId.get(edge.from)!;
    const to = ctx.byId.get(edge.to)!;
    if (!ctx.inDirs(from.file, uiDirs) || !ctx.inDirs(to.file, stateDirs)) continue;
    findings.push({ rule: 'ui-writes-state', severity: 'high', unit: from.id, file: from.file, line: from.start.line,
      message: `${from.name} writes ${to.name} (${to.file}:${to.start.line})` });
  }
  return findings;
}

export function nondeterminismInSimulation(ctx: AuditContext): RuleOutcome {
  if (ctx.options.simDirs.length === 0) return { skipped: 'needs --sim-dir' };
  return ctx.ir.units
    .filter((u) => ctx.inDirs(u.file, ctx.options.simDirs) && u.facts.nondeterminism.length > 0)
    .map((u): Finding => ({ rule: 'nondeterminism-in-simulation', severity: 'high', unit: u.id, file: u.file, line: u.start.line,
      message: `${u.name} uses ${u.facts.nondeterminism.join(', ')}` }));
}
