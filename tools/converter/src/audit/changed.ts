// The audit of a port judges the port, not the game it lands in: a finding counts when the unit it names (or its
// line, when it names none) touches a line the port added or changed since its base. The lines come from git: the
// diff of the working tree against the base, and files not tracked yet, whole.
import { spawnSync } from 'node:child_process';
import { MAX_CHILD_OUTPUT_BYTES } from '../common/files.ts';
import type { GraphIR } from '../graph/ir.ts';
import type { Finding } from './context.ts';

/** Inclusive line ranges, by file relative to the repository. */
export type ChangedLines = ReadonlyMap<string, readonly (readonly [number, number])[]>;

function git(repo: string, args: readonly string[]): string {
  const run = spawnSync('git', ['-C', repo, ...args], { encoding: 'utf8', maxBuffer: MAX_CHILD_OUTPUT_BYTES });
  if (run.status !== 0) throw new Error(`git ${args.join(' ')} in ${repo} failed (${run.status}):\n${run.stderr}`);
  return run.stdout;
}

const HUNK = /^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@/;

export function changedLines(repo: string, base: string): ChangedLines {
  const ranges = new Map<string, [number, number][]>();
  const add = (file: string, from: number, to: number): void => {
    (ranges.get(file) ?? ranges.set(file, []).get(file)!).push([from, to]);
  };
  let file: string | null = null;
  for (const line of git(repo, ['diff', '-U0', '--no-color', '--no-ext-diff', base]).split(/\r?\n/)) {
    if (line.startsWith('+++ ')) file = line === '+++ /dev/null' ? null : line.slice('+++ b/'.length);
    const hunk = HUNK.exec(line);
    if (hunk && file !== null) {
      const start = Number(hunk[1]);
      // A hunk that only removes lines (count 0) changes the code around the line it names.
      const count = hunk[2] === undefined ? 1 : Number(hunk[2]);
      add(file, start, start + Math.max(count, 1) - 1);
    }
  }
  for (const untracked of git(repo, ['ls-files', '--others', '--exclude-standard']).split(/\r?\n/).filter(Boolean)) {
    add(untracked, 1, Number.MAX_SAFE_INTEGER);
  }
  return ranges;
}

export function findingsInChanges(findings: readonly Finding[], ir: GraphIR, changed: ChangedLines): Finding[] {
  const units = new Map(ir.units.map((u) => [u.id, u]));
  const touches = (file: string, from: number, to: number): boolean => (changed.get(file) ?? []).some(([a, b]) => a <= to && from <= b);
  return findings.filter((f) => {
    const unit = f.unit === null ? undefined : units.get(f.unit);
    return unit ? touches(unit.file, unit.start.line, unit.end.line) : touches(f.file, f.line, f.line);
  });
}
