// What the feature uses of src/core/polyline.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/polyline.ts:36-363 class Polyline
/** An immutable polyline with lazily-built arc-length, tangent and bbox tables. */
export class Polyline { … }

// src/core/polyline.ts:123-136 getter Polyline.length
/** DO NOT make the arc-length table for the length alone (a road's four trimmed centrelines are asked their length when made, and kept: a typed array each, ~200 B before its numbers, 2026-10-21): the same sum, kept as one number. */
  get length(): number;

// src/core/polyline.ts:330-344 method Polyline.closestPoint
/** Nearest point on the polyline to `p`. */
  closestPoint(p: Vec2): { point: Vec2; s: number; distance: number };
