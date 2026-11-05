// Where code is in the graph: the units of each file, the innermost unit holding a position, and the node a unit was
// recorded from. One owner for every question of the feature stages that goes from source positions to units.
import path from 'node:path';
import type * as TS from 'typescript';
import type { GraphIR, UnitRecord } from '../graph/ir.ts';
import type { SourceProgram } from '../graph/typescript.ts';

export class UnitIndex {
  readonly units: ReadonlyMap<string, UnitRecord>;
  readonly byFile: ReadonlyMap<string, readonly UnitRecord[]>;
  readonly #sp: SourceProgram;
  readonly #nodes = new Map<string, Map<string, TS.Node>>();

  constructor(sp: SourceProgram, ir: GraphIR) {
    this.#sp = sp;
    this.units = new Map(ir.units.map((u) => [u.id, u]));
    const byFile = new Map<string, UnitRecord[]>();
    for (const u of ir.units) (byFile.get(u.file) ?? byFile.set(u.file, []).get(u.file)!).push(u);
    this.byFile = byFile;
  }

  /** A source file's path as the graph names it (relative to the source root). */
  relative(sf: TS.SourceFile): string {
    return path.relative(this.#sp.root, sf.fileName).split(path.sep).join('/');
  }

  sourceFile(file: string): TS.SourceFile | undefined {
    return this.#sp.sourceFile(path.join(this.#sp.root, file));
  }

  /** The smallest unit whose range holds a node's start. */
  unitAt(node: TS.Node): UnitRecord | null {
    const sf = node.getSourceFile();
    const at = sf.getLineAndCharacterOfPosition(node.getStart(sf));
    const line = at.line + 1;
    const column = at.character + 1;
    let best: UnitRecord | null = null;
    for (const u of this.byFile.get(this.relative(sf)) ?? []) {
      const inside = (line > u.start.line || (line === u.start.line && column >= u.start.column))
        && (line < u.end.line || (line === u.end.line && column <= u.end.column));
      if (inside && (!best || u.lines < best.lines || (u.lines === best.lines && u.start.column >= best.start.column))) best = u;
    }
    return best;
  }

  /** The node a unit was recorded from: the one that starts and ends where the unit does. */
  nodeOf(unit: UnitRecord): TS.Node | null {
    const sf = this.sourceFile(unit.file);
    if (!sf) return null;
    let nodes = this.#nodes.get(unit.file);
    if (!nodes) {
      // Each file is walked once: every node by where it starts and ends (the outermost, as a unit is recorded).
      const ts = this.#sp.ts;
      const found = new Map<string, TS.Node>();
      const visit = (n: TS.Node): void => {
        const key = `${n.getStart(sf)}:${n.end}`;
        if (!found.has(key)) found.set(key, n);
        ts.forEachChild(n, visit);
      };
      visit(sf);
      nodes = found;
      this.#nodes.set(unit.file, nodes);
    }
    const start = sf.getPositionOfLineAndCharacter(unit.start.line - 1, unit.start.column - 1);
    const end = sf.getPositionOfLineAndCharacter(unit.end.line - 1, unit.end.column - 1);
    return nodes.get(`${start}:${end}`) ?? null;
  }
}
