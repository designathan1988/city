// The feature's code as the source has it. A file every declaration of which is the feature's is written whole. From
// a file it shares with the program: its own declarations, each headed by its line range, and its slices of shared
// declarations (the lines that are the feature's, with `…` where shared code is left out). Files it imports that are
// not code (style sheets, data) are copied as they are.
import fs from 'node:fs';
import path from 'node:path';
import type { GraphIR, UnitRecord } from '../graph/ir.ts';
import type { FeatureBoundary } from './boundary.ts';
import type { FeatureResource } from './resources.ts';
import type { FeatureSlice } from './slice.ts';

export interface SourceOut {
  readonly file: string;
  /** whole: the file as it is; part: the feature's declarations and slices of it; resource: a file it imports. */
  readonly how: 'whole' | 'part' | 'resource';
  /** The lines of the file that are the feature's. */
  readonly lines: number;
}

export function writeSources(ir: GraphIR, b: FeatureBoundary, slices: readonly FeatureSlice[], resources: readonly FeatureResource[],
  sourceRoot: string, dir: string): SourceOut[] {
  const own = new Set(b.own);
  const byFile = new Map<string, UnitRecord[]>();
  for (const u of ir.units) (byFile.get(u.file) ?? byFile.set(u.file, []).get(u.file)!).push(u);
  const slicesOf = new Map<string, FeatureSlice[]>();
  for (const s of slices) (slicesOf.get(s.file) ?? slicesOf.set(s.file, []).get(s.file)!).push(s);
  const write = (file: string, text: string): void => {
    const target = path.join(dir, file);
    fs.mkdirSync(path.dirname(target), { recursive: true });
    fs.writeFileSync(target, text, 'utf8');
  };
  const result: SourceOut[] = [];
  for (const [file, units] of [...byFile].sort(([a], [c]) => a.localeCompare(c))) {
    const mine = units.filter((u) => own.has(u.id));
    const cut = slicesOf.get(file) ?? [];
    if (mine.length === 0 && cut.length === 0) continue;
    const text = fs.readFileSync(path.join(sourceRoot, file), 'utf8');
    if (mine.length === units.length) {
      write(file, text);
      result.push({ file, how: 'whole', lines: text.split('\n').length });
      continue;
    }
    const lines = text.split('\n');
    // A declaration inside another own one is written with it.
    const top = mine.filter((u) => !(u.parent !== null && own.has(u.parent)));
    const parts = [
      ...top.map((u) => ({ at: u.start.line, text: `// ${file}:${u.start.line}-${u.end.line} ${u.kind} ${u.name}: the feature's own\n${lines.slice(u.start.line - 1, u.end.line).join('\n')}` })),
      ...cut.map((s) => ({ at: s.start, text: renderSlice(s, lines) })),
    ].sort((x, y) => x.at - y.at);
    write(file, `// The code of ${file} that is the feature's (the rest of the file is shared).\n\n${parts.map((p) => p.text).join('\n\n')}\n`);
    result.push({ file, how: 'part', lines: top.reduce((n, u) => n + u.lines, 0) + cut.reduce((n, s) => n + s.lines.length, 0) });
  }
  // Files the feature imports that are not code, when they are the project's own files.
  for (const r of resources.filter((x) => x.kind === 'import' && x.key.startsWith('.'))) {
    const file = path.posix.normalize(path.posix.join(path.posix.dirname(r.file), r.key.replace(/\?.*$/, '')));
    const full = path.join(sourceRoot, file);
    if (!fs.existsSync(full) || result.some((s) => s.file === file)) continue;
    const text = fs.readFileSync(full, 'utf8');
    write(file, text);
    result.push({ file, how: 'resource', lines: text.split('\n').length });
  }
  return result;
}

/** A slice as the code it was cut from: its lines, and a `…` line with the count where code is left out. */
function renderSlice(s: FeatureSlice, lines: readonly string[]): string {
  const out = [`// ${s.file}:${s.start}-${s.end} slice of ${s.unit} (shared): the feature's statements; "…" marks code left out`];
  let previous = s.start - 1;
  for (const l of s.lines) {
    if (l > previous + 1) out.push(`${/^\s*/.exec(lines[l - 1] ?? '')![0]}// … ${l - previous - 1} lines`);
    out.push(lines[l - 1] ?? '');
    previous = l;
  }
  return out.join('\n');
}
