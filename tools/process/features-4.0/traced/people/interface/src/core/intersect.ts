// What the feature uses of src/core/intersect.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/intersect.ts:5-18 function lineLine
/** Intersection of the infinite line through `a0` with direction `da` and the infinite line through `b0` with direction */
export function lineLine(
  a0: Vec2,
  da: Vec2,
  b0: Vec2,
  db: Vec2,
  sinEps = EPS,
): { point: Vec2; sA: number; sB: number } | null;
