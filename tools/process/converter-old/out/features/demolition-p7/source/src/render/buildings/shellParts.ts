// The code of src/render/buildings/shellParts.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/shellParts.ts:62-73 function shellUv: the feature's own
export function shellUv(px: number, py: number, pz: number, nx: number, ny: number, nz: number): [number, number] {
  // The map's axes: x, y north, z up.
  const x = px, y = -pz, z = py;
  const length = Math.hypot(nx, ny, nz) || 1;
  const wx = nx / length, wy = -nz / length, wz = ny / length;
  const flat = Math.hypot(wx, wy);
  const tx = flat > 1e-6 ? -wy / flat : 1;
  const ty = flat > 1e-6 ? wx / flat : 0;
  // n x t: straight up on a wall, up the slope on a roof, +y on a flat.
  const bx = -wz * ty, by = wz * tx, bz = wx * ty - wy * tx;
  return [x * tx + y * ty, x * bx + y * by + z * bz];
}
