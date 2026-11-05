// The code of src/render/canyonLight.ts that is the feature's (the rest of the file is shared).

// src/render/canyonLight.ts:37-46 slice of src/render/canyonLight.ts#CanyonLight (shared): the feature's statements; "…" marks code left out
export interface CanyonLight {
  // … 2 lines
  follow(cx: number, cy: number, solids: (() => CameraSolids) | null, changed: readonly (readonly [number, number, number, number])[] | null): void;
// … 5 lines
}

// src/render/canyonLight.ts:271-295 slice of src/render/canyonLight.ts#createCanyonLight.layVolume (shared): the feature's statements; "…" marks code left out
  const layVolume: SolidVisit = (bx, by, c, s, lx0, ly0, lx1, ly1, top) => {
    const gx = laid!.gx, gy = laid!.gy;
    let minX = Infinity, minY = Infinity, maxX = -Infinity, maxY = -Infinity;
    // The box's four corners in the world, written out (no array a corner: hundreds of thousands a window).
    for (let corner = 0; corner < 4; corner++) {
      const lx = corner === 0 || corner === 3 ? lx0 : lx1, ly = corner < 2 ? ly0 : ly1;
      const wx = bx + lx * c - ly * s, wy = by + lx * s + ly * c;
      minX = Math.min(minX, wx); maxX = Math.max(maxX, wx); minY = Math.min(minY, wy); maxY = Math.max(maxY, wy);
    }
    const v = Math.max(1, Math.min(65535, Math.round((top + OFFSET) / STEP)));
    const i0 = Math.max(gx, Math.floor(minX / CELL)), i1 = Math.min(gx + SIDE - 1, Math.floor(maxX / CELL));
    const j0 = Math.max(gy, Math.floor(minY / CELL)), j1 = Math.min(gy + SIDE - 1, Math.floor(maxY / CELL));
    for (let j = j0; j <= j1; j++) {
      const row = j & MASK;
      for (let k = i0; k <= i1; k++) {
        // The cell's centre in the building's frame, inside the volume's box.
        const dx = (k + 0.5) * CELL - bx, dy = (j + 0.5) * CELL - by;
        const lx = dx * c + dy * s, ly = -dx * s + dy * c;
        if (lx < lx0 || lx > lx1 || ly < ly0 || ly > ly1) continue;
        const at = (row * SIDE + (k & MASK)) * 4;
        const was = data[at]! * 256 + data[at + 1]!;
        if (v > was) { data[at] = v >> 8; data[at + 1] = v & 255; markRow(row); }
      }
    }
  };

// src/render/canyonLight.ts:297-312 slice of src/render/canyonLight.ts#createCanyonLight.clearCells (shared): the feature's statements; "…" marks code left out
  const clearCells = (i0: number, j0: number, i1: number, j1: number): Rect | null => {
    const { gx, gy } = laid!;
    i0 = Math.max(i0, gx); i1 = Math.min(i1, gx + SIDE - 1);
    j0 = Math.max(j0, gy); j1 = Math.min(j1, gy + SIDE - 1);
    if (i0 > i1 || j0 > j1) return null;
    const from = i0 & MASK, count = i1 - i0 + 1;
    // A span that wraps round the torus is two runs of the row.
    const first = Math.min(count, SIDE - from);
    for (let j = j0; j <= j1; j++) {
      const row = j & MASK;
      data.fill(0, (row * SIDE + from) * 4, (row * SIDE + from + first) * 4);
      if (first < count) data.fill(0, row * SIDE * 4, (row * SIDE + count - first) * 4);
      markRow(row);
    }
    return [i0 * CELL, j0 * CELL, (i1 + 1) * CELL, (j1 + 1) * CELL];
  };

// src/render/canyonLight.ts:325-360 slice of src/render/canyonLight.ts#createCanyonLight.follow (shared): the feature's statements; "…" marks code left out
    follow(cx, cy, solids, changed) {
      // … 2 lines
      if (!laid || changed === null || Math.abs(gx - laid.gx) >= SIDE || Math.abs(gy - laid.gy) >= SIDE) {
        // … 4 lines
        solids().forEachIn(gx * CELL, gy * CELL, (gx + SIDE) * CELL, (gy + SIDE) * CELL, layVolume);
      // … 5 lines
      }
      const rects: Rect[] = [];
      if (gx !== laid.gx || gy !== laid.gy) {
        // … 8 lines
        if (columns) rects.push(columns);
        if (rows) rects.push(rows);
      }
      // … 1 lines
      for (const rect of changed) {
        // … 1 lines
        if (cleared) rects.push(cleared);
      }
      if (rects.length) solids().forEachInAny(rects, layVolume);
    // … 1 lines
    },
