// Classifies a feature of the source for conversion: the role of every unit it owns, every source concept it depends
// on with the target unit that owns it (checked against the target's own graph), the constants that need a source,
// the methodology rewrites the translation must apply, and the units to split. A feature is ready to translate only
// when nothing is unmapped and nothing mixed remains.
import type { EdgeRecord, GraphIR, UnitRecord } from '../graph/ir.ts';
import { classifyRoles, type UnitRole } from './roles.ts';
import type { FeatureSpec, MappingCatalog } from './spec.ts';

export interface Dependency {
  readonly source: string;
  readonly usedBy: string[];
  readonly target: string | null;
  readonly problem: string | null;
}

export interface Classification {
  readonly feature: string;
  readonly subsystem: string;
  readonly units: UnitRole[];
  readonly dependencies: Dependency[];
  /** External APIs the feature uses (the translation's rule catalog must cover each). */
  readonly apis: string[];
  readonly constants: { readonly unit: string; readonly value: string; readonly line: number }[];
  readonly rewrites: { readonly unit: string; readonly rule: string; readonly line: number; readonly detail: string }[];
  readonly blocked: string[];
  readonly ready: boolean;
}

export function classifyFeature(source: GraphIR, target: GraphIR, spec: FeatureSpec, catalog: MappingCatalog): Classification {
  const files = new Set(spec.files.map((f) => f.replace(/\\/g, '/')));
  for (const file of files) if (!source.files.some((f) => f.path === file)) throw new Error(`${file} is not in the source graph`);
  const byId = new Map(source.units.map((u) => [u.id, u]));
  const outgoing = new Map<string, EdgeRecord[]>();
  for (const e of source.edges) outgoing.set(e.from, [...(outgoing.get(e.from) ?? []), e]);
  const own: UnitRecord[] = source.units.filter((u) => files.has(u.file));
  const ownIds = new Set(own.map((u) => u.id));
  const units = classifyRoles(own, byId, outgoing);

  const targetIds = new Set(target.units.map((u) => u.id));
  const mapped = new Map(catalog.concepts.map((c) => [c.source, c.target]));
  const usedBy = new Map<string, Set<string>>();
  for (const unit of own) {
    for (const edge of outgoing.get(unit.id) ?? []) {
      if (ownIds.has(edge.to)) continue;
      usedBy.set(edge.to, (usedBy.get(edge.to) ?? new Set()).add(unit.id));
    }
  }
  const dependencies: Dependency[] = [...usedBy].map(([dep, users]) => {
    const to = mapped.get(dep) ?? null;
    const problem = to === null ? 'no target owner mapped' : targetIds.has(to) ? null : `mapped to ${to}, which the target graph does not have`;
    return { source: dep, usedBy: [...users].sort(), target: to, problem };
  }).sort((a, b) => a.source.localeCompare(b.source));

  const apis = [...new Set(own.flatMap((u) => u.facts.external.map((e) => e.key)))].sort();
  const constants = own.flatMap((u) => u.facts.numericLiterals.filter((l) => l.context === 'constant').map((l) => ({ unit: u.id, value: l.text, line: l.line })));
  const rewrites = own.flatMap((u) => [
    ...u.facts.patterns.map((p) => ({ unit: u.id, rule: p.id, line: p.line, detail: p.detail })),
    ...u.facts.loopAllocations.map((a) => ({ unit: u.id, rule: 'hot-allocation', line: a.line, detail: a.what })),
  ]);
  const blocked = [
    ...dependencies.filter((d) => d.problem).map((d) => `${d.source}: ${d.problem}`),
    ...units.filter((u) => u.role === 'mixed').map((u) => `${u.id}: mixes ${u.parts.join(' and ')} (${u.why}); split it by responsibility`),
  ];
  return { feature: spec.feature, subsystem: spec.subsystem, units, dependencies, apis, constants, rewrites, blocked, ready: blocked.length === 0 };
}

export function renderClassification(c: Classification): string {
  const count = (role: string): number => c.units.filter((u) => u.role === role).length;
  return [
    `# Classification: ${c.feature} → ${c.subsystem}`, '',
    c.ready ? 'Ready to translate.' : `**Blocked** by ${c.blocked.length} item(s).`, '',
    `Units: ${c.units.length} (${['state', 'tick', 'compute', 'data', 'render', 'ui', 'type', 'mixed'].map((r) => `${count(r)} ${r}`).join(', ')}).`,
    `Dependencies: ${c.dependencies.length} (${c.dependencies.filter((d) => !d.problem).length} mapped). External APIs: ${c.apis.length}. ` +
      `Constants needing a source: ${c.constants.length}. Rewrites: ${c.rewrites.length}.`, '',
    '## Blocking', '', ...(c.blocked.length ? c.blocked.map((b) => `- ${b}`) : ['Nothing.']), '',
    '## Units', '', '| Unit | Role | Why |', '|---|---|---|', ...c.units.map((u) => `| ${u.id} | ${u.role} | ${u.why} |`), '',
    '## Dependencies', '', '| Source concept | Used by | Target owner |', '|---|---|---|',
    ...c.dependencies.map((d) => `| ${d.source} | ${d.usedBy.length} | ${d.target ?? '—'}${d.problem ? ` (${d.problem})` : ''} |`), '',
    '## Rewrites the translation must apply', '', ...c.rewrites.map((r) => `- ${r.unit}:${r.line} — ${r.rule}: ${r.detail}`), '',
    '## Constants that need a measurement or a primary source', '', ...c.constants.map((k) => `- ${k.unit}:${k.line} — ${k.value}`), '',
  ].join('\n');
}
