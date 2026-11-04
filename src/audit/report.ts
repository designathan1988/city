// Readable audit report: a summary table, then for each rule the problem, the correct methodology, its target
// counterpart, the sources, and the findings (the JSON keeps all of them).
import { RULE_BY_ID } from './catalog.ts';
import type { AuditResult } from './run.ts';

const SHOWN_PER_RULE = 40;

export function renderAudit(result: AuditResult): string {
  const out: string[] = [
    '# Audit',
    '',
    `Source: ${result.sourceRoot} (${result.graphUnits} units). Hot path: ${result.hot.units} units reachable from ${result.hot.roots.length} root(s).`,
    '',
    '| Rule | High | Medium | Low | Status |',
    '|---|---|---|---|---|',
    ...result.rules.map((r) => `| ${r.id} | ${r.bySeverity.high} | ${r.bySeverity.medium} | ${r.bySeverity.low} | ${r.skipped ? `not run: ${r.skipped}` : 'run'} |`),
    '',
  ];
  if (result.features.length > 0) {
    out.push('## Features', '', '| Feature | Units | Files | Degree of scattering |', '|---|---|---|---|',
      ...result.features.map((f) => `| ${f.name} | ${f.units} | ${f.files} | ${f.degreeOfScattering} |`), '');
  }
  for (const summary of result.rules) {
    const rule = RULE_BY_ID.get(summary.id)!;
    const findings = result.findings.filter((f) => f.rule === summary.id);
    out.push(`## ${rule.title} (\`${rule.id}\`)`, '');
    out.push(`- **Problem:** ${rule.problem}`, `- **Correct methodology:** ${rule.methodology}`, `- **In the target:** ${rule.target}`);
    out.push(`- **Sources:** ${rule.sources.map((s) => `[${s.title}](${s.url})`).join('; ')}`, '');
    if (summary.skipped) {
      out.push(`Not run: ${summary.skipped}.`, '');
      continue;
    }
    if (findings.length === 0) {
      out.push('No findings.', '');
      continue;
    }
    for (const f of findings.slice(0, SHOWN_PER_RULE)) out.push(`- [${f.severity}] ${f.file}:${f.line} — ${f.message}`);
    if (findings.length > SHOWN_PER_RULE) out.push(`- ... and ${findings.length - SHOWN_PER_RULE} more (all in the JSON)`);
    out.push('');
  }
  return out.join('\n');
}
