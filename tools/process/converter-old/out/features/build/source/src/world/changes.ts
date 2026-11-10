// The code of src/world/changes.ts that is the feature's (the rest of the file is shared).

// src/world/changes.ts:141-145 function rectsAround: the feature's own
export function rectsAround(rects: readonly ChangeRect[]): ChangeRect {
  let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
  for (const r of rects) { x0 = Math.min(x0, r[0]); y0 = Math.min(y0, r[1]); x1 = Math.max(x1, r[2]); y1 = Math.max(y1, r[3]); }
  return [x0, y0, x1, y1];
}
