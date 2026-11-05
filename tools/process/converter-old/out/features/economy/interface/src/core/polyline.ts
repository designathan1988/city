// What the feature uses of src/core/polyline.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/polyline.ts:7-20 interface Frame
/** A position on a polyline, with its local frame. */
export interface Frame {
  /** Position in world space. */
  readonly p: Vec2;
  /** Unit tangent, pointing forward along the polyline. */
  readonly t: Vec2;
  /** Unit left normal, `perp(t)`. */
  readonly n: Vec2;
  /** Index of the containing segment. */
  readonly i: number;
  /** Parametric position within that segment, in [0,1]. */
  readonly u: number;
  /** Arc length from the start. */
  readonly s: number;
}

// src/core/polyline.ts:36-363 class Polyline
/** An immutable polyline with lazily-built arc-length, tangent and bbox tables. */
export class Polyline { … }

// src/core/polyline.ts:123-136 getter Polyline.length
/** DO NOT make the arc-length table for the length alone (a road's four trimmed centrelines are asked their length when made, and kept: a typed array each, ~200 B before its numbers, 2026-10-21): the same sum, kept as one number. */
  get length(): number;

// src/core/polyline.ts:152-154 method Polyline.sampleAt
/** Frame at arc length `s`, clamped to the polyline. */
  sampleAt(s: number): Frame;
