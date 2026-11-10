// What the feature uses of src/render/riderPoses.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/riderPoses.ts:52-57 interface WheelSpec
/** A steering wheel, from the hip point, metres: its centre ahead and above, the tilt of its rim from vertical */
export interface WheelSpec {
  readonly forward: number;
  readonly up: number;
  readonly tilt: number;
  readonly radius: number;
}
