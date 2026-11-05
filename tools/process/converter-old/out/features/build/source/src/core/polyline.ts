// The code of src/core/polyline.ts that is the feature's (the rest of the file is shared).

// src/core/polyline.ts:366-383 function segmentDistSq: the feature's own
export function segmentDistSq(xy: Float64Array, i: number, px: number, py: number): number {
  const ax = xy[i * 2] as number;
  const ay = xy[i * 2 + 1] as number;
  const abx = (xy[i * 2 + 2] as number) - ax;
  const aby = (xy[i * 2 + 3] as number) - ay;
  const l2 = abx * abx + aby * aby;
  let qx = ax;
  let qy = ay;
  if (l2 >= EPS) {
    let t = ((px - ax) * abx + (py - ay) * aby) / l2;
    t = t < 0 ? 0 : t > 1 ? 1 : t;
    qx = ax + abx * t;
    qy = ay + aby * t;
  }
  const ex = px - qx;
  const ey = py - qy;
  return ex * ex + ey * ey;
}
