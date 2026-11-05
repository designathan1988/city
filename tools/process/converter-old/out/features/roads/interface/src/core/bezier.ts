// What the feature uses of src/core/bezier.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/bezier.ts:5-10 interface CurveShape
/** A segment's curvature, stored so that dragging a node cannot deform it. */
export interface CurveShape {
  /** Control-point position along the chord, normalized to [0,1]. */
  readonly t: number;
  /** Control-point offset from the chord, in world units (signed). */
  readonly h: number;
}

// src/core/bezier.ts:13-18 function controlPoint
/** Reconstructs the quadratic control point from endpoints and shape. */
export function controlPoint(a: Vec2, b: Vec2, shape: CurveShape): Vec2;

// src/core/bezier.ts:59-67 function flattenSegment
/** Flattens a segment defined by endpoints plus an optional shape. */
export function flattenSegment(
  a: Vec2,
  b: Vec2,
  shape: CurveShape | null,
  tol = FLATTEN_TOL,
): Vec2[];

// src/core/bezier.ts:99-112 function fitShapeToRadius
/** The same curve, flattened just enough that it is nowhere tighter than `minRadius` */
export function fitShapeToRadius(a: Vec2, b: Vec2, shape: CurveShape | null, minRadius: number): CurveShape | null;
