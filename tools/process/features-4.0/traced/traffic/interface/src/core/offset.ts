// What the feature uses of src/core/offset.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/offset.ts:15-22 interface OffsetOptions
export interface OffsetOptions {
  /** Miter is replaced by a round join beyond this multiple of |d|. */
  readonly miterLimit?: number;
  /** Max chordal deviation for round joins, in world units. */
  readonly arcTol?: number;
  /** Remove loops produced on the inner side of tight curves. */
  readonly prune?: boolean;
}

// src/core/offset.ts:46-112 function offsetPolyline
/** Offsets an open polyline by a signed distance `d` along `perp(tangent)`. */
export function offsetPolyline(
  pts: readonly Vec2[],
  d: number,
  opts: OffsetOptions = {},
): Vec2[];
