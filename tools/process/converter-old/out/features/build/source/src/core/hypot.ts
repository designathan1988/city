// The code of src/core/hypot.ts that is the feature's (the rest of the file is shared).

// src/core/hypot.ts:27-48 function hypot3: the feature's own
export function hypot3(dx: number, dy: number, dz: number): number {
  const ax = Math.abs(dx), ay = Math.abs(dy), az = Math.abs(dz);
  const nanX = dx !== dx, nanY = dy !== dy, nanZ = dz !== dz;
  let max = 0;
  if (!nanX && ax > max) max = ax;
  if (!nanY && ay > max) max = ay;
  if (!nanZ && az > max) max = az;
  if (max === Infinity) return Infinity;
  if (nanX || nanY || nanZ) return NaN;
  if (max === 0) return 0;
  // Kahan summation of the squares of each over the largest, in the arguments' order.
  let sum = 0, compensation = 0;
  const nx = ax / max, sx = nx * nx - compensation, px = sum + sx;
  compensation = (px - sum) - sx;
  sum = px;
  const ny = ay / max, sy = ny * ny - compensation, py = sum + sy;
  compensation = (py - sum) - sy;
  sum = py;
  const nz = az / max, sz = nz * nz - compensation, pz = sum + sz;
  sum = pz;
  return Math.sqrt(sum) * max;
}
