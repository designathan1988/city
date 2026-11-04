// The feature's own code, as the source has it: a file every declaration of which is the feature's is written whole;
// from a file it shares with other code, only its own declarations, each headed by its line range.
import fs from 'node:fs';
import path from 'node:path';
import type { GraphIR, UnitRecord } from '../graph/ir.ts';
import type { FeatureBoundary } from './boundary.ts';

export interface SourceOut {
  readonly file: string;
  readonly whole: boolean;
  readonly lines: number;
}

/** The feature's own code, as it is in the source: whole files, or the top-level own declarations of a shared file. */
export function writeSources(ir: GraphIR, b: FeatureBoundary, sourceRoot: string, dir: string): SourceOut[] {
  const own = new Set(b.own);
  const byFile = new Map<string, UnitRecord[]>();
  for (const u of ir.units) (byFile.get(u.file) ?? byFile.set(u.file, []).get(u.file)!).push(u);
  const result: SourceOut[] = [];
  for (const [file, units] of [...byFile].sort(([a], [b]) => a.localeCompare(b))) {
    const mine = units.filter((u) => own.has(u.id));
    if (mine.length === 0) continue;
    const text = fs.readFileSync(path.join(sourceRoot, file), 'utf8');
    const target = path.join(dir, file);
    fs.mkdirSync(path.dirname(target), { recursive: true });
    if (mine.length === units.length) {
      fs.writeFileSync(target, text, 'utf8');
      result.push({ file, whole: true, lines: text.split('\n').length });
      continue;
    }
    // A declaration inside another own one is written with it.
    const top = mine.filter((u) => !(u.parent !== null && own.has(u.parent))).sort((x, y) => x.start.line - y.start.line);
    const lines = text.split('\n');
    const parts = top.map((u) => `// ${file}:${u.start.line}-${u.end.line} ${u.kind} ${u.name}\n${lines.slice(u.start.line - 1, u.end.line).join('\n')}`);
    fs.writeFileSync(target, `// The declarations of ${file} that are the feature's own (the rest of the file is shared).\n\n${parts.join('\n\n')}\n`, 'utf8');
    result.push({ file, whole: false, lines: top.reduce((n, u) => n + u.lines, 0) });
  }
  return result;
}
