// The code of src/world/cityGen/squares.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/squares.ts:17-22 slice of src/world/cityGen/squares.ts#SquarePlan (shared): the feature's statements; "…" marks code left out
export interface SquarePlan {
  // … 2 lines
  readonly dabs: readonly PaintDab[];
  readonly trees: readonly PlantedTree[];
}

// src/world/cityGen/squares.ts:100-152 slice of src/world/cityGen/squares.ts#planSquares (shared): the feature's statements; "…" marks code left out
export function planSquares(doc: RoadDoc, seed: number): SquarePlan[] {
  // … 20 lines
  for (const { block, centre } of [...byCell.values()].sort((a, b) => a.centre.x - b.centre.x || a.centre.y - b.centre.y)) {
    // … 11 lines
    const dabs: PaintDab[] = [];
    // … 1 lines
    for (let x = minX; x <= maxX; x += m(1.8)) for (let y = minY; y <= maxY; y += m(1.8)) {
      // … 1 lines
      if (inside(p) && onWalk(p)) dabs.push({ kind: 'concrete', x, y, radius: m(1.4), strength: 1 });
    }
    // … 2 lines
    const trees: PlantedTree[] = [];
    for (let a = -m(200); a <= m(200); a += m(9)) for (let b = -m(200); b <= m(200); b += m(9)) {
      // … 4 lines
      trees.push({ x: p.x, y: p.y, height: m(9 + random() * 5), yaw: random() * Math.PI * 2, seed: treeSeed('mixed', random) });
    }
    out.push({ lots: block.map((l) => l.id), centre, dabs, trees });
  }
// … 1 lines
}
