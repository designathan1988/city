// The code of src/render/buildings/cellArrays.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/cellArrays.ts:126-132 function octDecode: the feature's own
export function octDecode(a: number, b: number): [number, number, number] {
  let x = Math.max(-1, a / NORMAL_UNIT), y = Math.max(-1, b / NORMAL_UNIT);
  const z = 1 - Math.abs(x) - Math.abs(y);
  if (z < 0) { const nx = (1 - Math.abs(y)) * (x >= 0 ? 1 : -1), ny = (1 - Math.abs(x)) * (y >= 0 ? 1 : -1); x = nx; y = ny; }
  const l = Math.hypot(x, y, z) || 1;
  return [x / l, y / l, z / l];
}
