// What the feature uses of src/editor/roadPath.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roadPath.ts:6-9 interface RoadPathPoint
/** An authored point. Heights are offsets from the designed ground in world units. */
export interface RoadPathPoint {
  readonly at: Vec2;
  readonly heightOffset: number;
}

// src/editor/roadPath.ts:12-16 interface RoadPathPiece
/** One quadratic in a continuous road gesture. Neighbouring pieces share an end. */
export interface RoadPathPiece {
  readonly start: RoadPathPoint;
  readonly end: RoadPathPoint;
  readonly curve: CurveShape | null;
}
