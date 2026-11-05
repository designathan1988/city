// What the feature uses of src/sim/pose.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/pose.ts:87-93 interface VehicleFrames
/** A vehicle's two poses of the tick as `vehiclePoseInto` blends them: the simulation's worker writes these, the page blends. */
export interface VehicleFrames {
  bx: number; by: number; bangle: number;
  x: number; y: number; angle: number;
  yawBefore: number; yawNow: number;
  /** No blend: the pose now alone (no pose before, or a jump past `POSE_JUMP_LIMIT`). */
  jump: boolean;
}
