// What the feature uses of src/core/intersect.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/intersect.ts:21-36 function segSeg
/** Intersection of the finite segments a->b and c->d. */
export function segSeg(
  a: Vec2,
  b: Vec2,
  c: Vec2,
  d: Vec2,
): { point: Vec2; t: number; u: number } | null;
