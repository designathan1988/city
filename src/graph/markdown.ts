// Human-readable view of a graph IR: the call tree of the callable units and the code of every unit, by file.
// It is rendered from the IR (not from the AST), so what a person reads is exactly what the later stages consume.
import fs from 'node:fs';
import path from 'node:path';
import type { EdgeKind, GraphIR, UnitRecord } from './ir.ts';

const TREE_KINDS = new Set<EdgeKind>(['call', 'construct', 'implementation', 'callbackAssign', 'moduleLoad']);

function label(unit: UnitRecord): string {
  return `${unit.name} (${unit.file}:${unit.start.line})`;
}

function tree(ir: GraphIR, byId: Map<string, UnitRecord>): string[] {
  const children = new Map<string, string[]>();
  const incoming = new Set<string>();
  for (const edge of ir.edges) {
    if (!TREE_KINDS.has(edge.kind) || !byId.get(edge.to)?.facts.callable) continue;
    const list = children.get(edge.from) ?? [];
    if (!list.includes(edge.to)) list.push(edge.to);
    children.set(edge.from, list);
    incoming.add(edge.to);
  }
  const roots = ir.units.filter((u) => u.facts.callable && !incoming.has(u.id)).map((u) => u.id);
  const out: string[] = [];
  const printed = new Set<string>();
  const print = (id: string, depth: number, path: Set<string>): void => {
    const unit = byId.get(id)!;
    const repeated = printed.has(id);
    out.push(`${'  '.repeat(depth)}${label(unit)}${repeated ? '  (listed above)' : ''}`);
    if (repeated) return;
    printed.add(id);
    for (const child of children.get(id) ?? []) if (!path.has(child)) print(child, depth + 1, new Set([...path, child]));
  };
  for (const root of roots) print(root, 0, new Set([root]));
  return out;
}

function code(ir: GraphIR): string[] {
  const out: string[] = [];
  const byFile = new Map<string, UnitRecord[]>();
  for (const unit of ir.units) byFile.set(unit.file, [...(byFile.get(unit.file) ?? []), unit]);
  const firstMemberLine = new Map<string, number>();
  for (const unit of ir.units) {
    if (unit.parent !== null) firstMemberLine.set(unit.parent, Math.min(firstMemberLine.get(unit.parent) ?? Infinity, unit.start.line));
  }
  for (const [file, units] of [...byFile].sort(([a], [b]) => a.localeCompare(b))) {
    const lines = fs.readFileSync(path.join(ir.sourceRoot, file), 'utf8').split(/\r?\n/);
    out.push(`### ${file}`, '', '```ts');
    let printedUpTo = 0;
    for (const unit of [...units].sort((a, b) => a.start.line - b.start.line)) {
      if (unit.start.line <= printedUpTo && unit.kind !== 'class') continue; // nested in a unit already printed
      // A class head prints its declaration line(s) only; its members are units of their own.
      const last = unit.kind === 'class' ? Math.max(unit.start.line, (firstMemberLine.get(unit.id) ?? unit.end.line) - 1) : unit.end.line;
      out.push(`// line ${unit.start.line}`, ...lines.slice(unit.start.line - 1, last), '');
      if (unit.kind !== 'class') printedUpTo = Math.max(printedUpTo, last);
    }
    out.push('```', '');
  }
  return out;
}

export function renderMarkdown(ir: GraphIR): string {
  const byId = new Map(ir.units.map((u) => [u.id, u]));
  const callables = ir.units.filter((u) => u.facts.callable).length;
  return [
    `# Graph: ${ir.entries.length > 0 ? ir.entries.join(', ') : `whole scope (${ir.include.join(', ')})`}`,
    '',
    `${ir.stats.units} units (${callables} callable) in ${ir.stats.files} files, ${ir.stats.edges} edges, ` +
      `${ir.external.length} external APIs; ${ir.stats.milliseconds} ms.`,
    '',
    '## Call tree',
    '',
    '```',
    ...tree(ir, byId),
    '```',
    '',
    '## Code by file',
    '',
    ...code(ir),
  ].join('\n');
}
