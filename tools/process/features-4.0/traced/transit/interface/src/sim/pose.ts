// What the feature uses of src/sim/pose.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/pose.ts:11-14 interface Pose
export interface Pose {
  readonly p: Vec2;
  readonly angle: number;
}

// src/sim/pose.ts:46-49 function vehiclePose
/** Pose of a vehicle, interpolated between the last two simulation steps. */
export function vehiclePose(w: SimWorld, v: Vehicle, alpha: number): Pose | null;
