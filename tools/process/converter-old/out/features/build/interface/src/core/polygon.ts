// What the feature uses of src/core/polygon.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/polygon.ts:8-27 function isSimple
/** True when a closed polygon has no self-intersections. DO NOT test every pair of edges with `segSeg` (it makes vectors each call: 21 s of the tile workers' junction rings in an opening, wprof5): each edge's box once, `segSeg` only where two boxes meet; the same answer. */
export function isSimple(points: readonly Vec2[]): boolean;

// src/core/polygon.ts:29-37 function signedArea
export function signedArea(points: readonly Vec2[]): number;

// src/core/polygon.ts:40-51 function pointInPolygon
/** Ray-casting containment test. Points exactly on an edge are unspecified. */
export function pointInPolygon(p: Vec2, points: readonly Vec2[]): boolean;

// src/core/polygon.ts:54-90 function convexHull
/** Monotone-chain convex hull, wound to positive signed area. */
export function convexHull(input: readonly Vec2[]): Vec2[];
