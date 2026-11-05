// What the feature uses of src/core/bezier.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/bezier.ts:5-10 interface CurveShape
/** A segment's curvature, stored so that dragging a node cannot deform it. */
export interface CurveShape {
  /** Control-point position along the chord, normalized to [0,1]. */
  readonly t: number;
  /** Control-point offset from the chord, in world units (signed). */
  readonly h: number;
}

// src/core/bezier.ts:99-112 function fitShapeToRadius
/** The same curve, flattened just enough that it is nowhere tighter than `minRadius` */
export function fitShapeToRadius(a: Vec2, b: Vec2, shape: CurveShape | null, minRadius: number): CurveShape | null;
