// The code of src/core/intersect.ts that is the feature's (the rest of the file is shared).

// src/core/intersect.ts:57-68 function inForwardSlab: the feature's own
export function inForwardSlab(
  p: Vec2,
  origin: Vec2,
  dir: Vec2,
  hw: number,
  margin = GEO_EPS,
): boolean {
  const w = sub(p, origin);
  const along = dot(w, dir);
  if (along <= margin) return false;
  return Math.abs(cross(dir, w)) < hw - margin;
}
