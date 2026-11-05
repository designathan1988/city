// The code of src/core/aabb.ts that is the feature's (the rest of the file is shared).

// src/core/aabb.ts:41-46 function expand: the feature's own
export const expand = (b: Aabb, m: number): Aabb => ({
  minX: b.minX - m,
  minY: b.minY - m,
  maxX: b.maxX + m,
  maxY: b.maxY + m,
});
