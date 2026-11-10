// The code of src/world/groundPatch.ts that is the feature's (the rest of the file is shared).

// src/world/groundPatch.ts:18-27 function latticeHeight: the feature's own
export function latticeHeight(x: number, y: number, cell: number, corner: (i: number, j: number) => number): number {
  let gx = x / cell, gy = -y / cell;
  const rx = Math.round(gx), ry = Math.round(gy);
  if (Math.abs(gx - rx) < ON_LINE) gx = rx;
  if (Math.abs(gy - ry) < ON_LINE) gy = ry;
  const i = Math.floor(gx), j = Math.floor(gy);
  const u = gx - i, v = gy - j;
  const a = corner(i, j), b = corner(i, j + 1), c = corner(i + 1, j + 1), d = corner(i + 1, j);
  return cellHeight(a, b, c, d, u, v, cellDiagonal(a, b, c, d));
}

// src/world/groundPatch.ts:30-38 interface GroundPatch: the feature's own
export interface GroundPatch {
  readonly cell: number;
  readonly i0: number;
  readonly j0: number;
  readonly cols: number;
  readonly rows: number;
  /** Row by row, `cols` a row. */
  readonly heights: Float64Array;
}

// src/world/groundPatch.ts:41-48 function sampleGroundPatch: the feature's own
export function sampleGroundPatch(rect: readonly [number, number, number, number], cell: number, groundAt: (x: number, y: number) => number): GroundPatch {
  const i0 = Math.floor(rect[0] / cell), i1 = Math.floor(rect[2] / cell) + 1;
  const j0 = Math.floor(-rect[3] / cell), j1 = Math.floor(-rect[1] / cell) + 1;
  const cols = i1 - i0 + 1, rows = j1 - j0 + 1;
  const heights = new Float64Array(cols * rows);
  for (let j = 0; j < rows; j++) for (let i = 0; i < cols; i++) heights[j * cols + i] = groundAt((i0 + i) * cell, -(j0 + j) * cell);
  return { cell, i0, j0, cols, rows, heights };
}

// src/world/groundPatch.ts:51-55 class PatchMiss: the feature's own
export class PatchMiss extends Error {
  constructor(readonly x: number, readonly y: number) {
    super(`the ground was read at (${x}, ${y}), off the patch sent`);
  }
}

// src/world/groundPatch.ts:58-65 function patchGround: the feature's own
export function patchGround(patch: GroundPatch): (x: number, y: number) => number {
  const { cell, i0, j0, cols, rows, heights } = patch;
  return (x, y) => latticeHeight(x, y, cell, (i, j) => {
    const ci = i - i0, cj = j - j0;
    if (ci < 0 || cj < 0 || ci >= cols || cj >= rows) throw new PatchMiss(x, y);
    return heights[cj * cols + ci]!;
  });
}
