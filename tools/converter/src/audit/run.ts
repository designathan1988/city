// Runs every rule of the catalog over a graph IR and gathers the result: findings sorted by severity, what each rule
// found or why it could not run, the hot set and the feature metrics.
import type { GraphIR } from '../graph/ir.ts';
import { duplicatedCode, duplicatedConstants, featureMetrics, featureScattering, godFiles, godFunctions, type FeatureMetrics } from './architecture.ts';
import { RULES } from './catalog.ts';
import { AuditContext, type AuditOptions, type Finding, type RuleOutcome, type Severity } from './context.ts';
import { hotAllocation, patternRule, perObjectDraw, undisposedGpuResources } from './performance.ts';
import { moduleState, nondeterminismInSimulation, scatteredWrites, uiWritesState } from './state.ts';

export const AUDIT_VERSION = 1;

export interface RuleSummary {
  readonly id: string;
  readonly title: string;
  readonly group: string;
  readonly findings: number;
  readonly bySeverity: Record<Severity, number>;
  readonly skipped: string | null;
}

export interface AuditResult {
  readonly version: number;
  readonly sourceRoot: string;
  readonly graphUnits: number;
  readonly options: AuditOptions;
  readonly hot: { readonly roots: string[]; readonly units: number; readonly members: string[] };
  readonly rules: RuleSummary[];
  readonly features: FeatureMetrics[];
  readonly findings: Finding[];
}

const SEVERITY_ORDER: Record<Severity, number> = { high: 0, medium: 1, low: 2 };

export function runAudit(ir: GraphIR, options: AuditOptions): AuditResult {
  const ctx = new AuditContext(ir, options);
  const metrics = featureMetrics(ctx);
  const outcomes: Record<string, () => RuleOutcome> = {
    'god-file': () => godFiles(ctx),
    'god-function': () => godFunctions(ctx),
    'module-state': () => moduleState(ctx),
    'scattered-writes': () => scatteredWrites(ctx),
    'ui-writes-state': () => uiWritesState(ctx),
    'duplicated-code': () => duplicatedCode(ctx),
    'duplicated-constant': () => duplicatedConstants(ctx),
    'feature-scattering': () => featureScattering(ctx, metrics),
    'nondeterminism-in-simulation': () => nondeterminismInSimulation(ctx),
    'queue-shift': () => patternRule(ctx, 'queue-shift'),
    'object-delete': () => patternRule(ctx, 'object-delete'),
    'holey-array': () => patternRule(ctx, 'holey-array'),
    'quadratic-proximity': () => patternRule(ctx, 'quadratic-proximity', 'medium'),
    'sync-gpu-readback': () => patternRule(ctx, 'sync-gpu-readback', 'medium'),
    'worker-copy': () => patternRule(ctx, 'worker-copy', 'medium'),
    'hot-allocation': () => hotAllocation(ctx),
    'per-object-draw': () => perObjectDraw(ctx),
    'undisposed-gpu-resource': () => undisposedGpuResources(ctx),
  };
  const findings: Finding[] = [];
  const rules: RuleSummary[] = RULES.map((rule) => {
    const run = outcomes[rule.id];
    if (!run) throw new Error(`rule ${rule.id} is in the catalog but has no detector`);
    const outcome = run();
    const bySeverity: Record<Severity, number> = { high: 0, medium: 0, low: 0 };
    if ('skipped' in outcome) return { id: rule.id, title: rule.title, group: rule.group, findings: 0, bySeverity, skipped: outcome.skipped };
    for (const finding of outcome) bySeverity[finding.severity]++;
    findings.push(...outcome);
    return { id: rule.id, title: rule.title, group: rule.group, findings: outcome.length, bySeverity, skipped: null };
  });
  for (const id of Object.keys(outcomes)) if (!RULES.some((r) => r.id === id)) throw new Error(`detector ${id} has no catalog entry`);
  findings.sort((a, b) => SEVERITY_ORDER[a.severity] - SEVERITY_ORDER[b.severity] || a.rule.localeCompare(b.rule) ||
    a.file.localeCompare(b.file) || a.line - b.line);
  return {
    version: AUDIT_VERSION,
    sourceRoot: ir.sourceRoot,
    graphUnits: ir.units.length,
    options,
    hot: { roots: ctx.hotRoots, units: ctx.hot.size, members: [...ctx.hot].sort() },
    rules,
    features: metrics,
    findings,
  };
}
