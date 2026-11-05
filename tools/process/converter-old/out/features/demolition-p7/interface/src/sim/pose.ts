// What the feature uses of src/sim/pose.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/pose.ts:17-20 interface PoseOut
/** A pose written in place (`vehiclePoseInto`): one per caller, made once. */
export interface PoseOut {
  p: { x: number; y: number };
  angle: number;
}

// src/sim/pose.ts:23-25 function newPose
/** A pose to write into. */
export function newPose(): PoseOut;

// src/sim/pose.ts:31-33 function laneChangeYaw
/** Angle a vehicle points into its own lane change. */
function laneChangeYaw(slope: number): number;

// src/sim/pose.ts:39-43 function bodyOffset
/** Sideways offset of the body CENTRE during a lane change. */
function bodyOffset(k: Kinematics, length: number): number;

// src/sim/pose.ts:87-93 interface VehicleFrames
/** A vehicle's two poses of the tick as `vehiclePoseInto` blends them: the simulation's worker writes these, the page blends. */
export interface VehicleFrames {
  bx: number; by: number; bangle: number;
  x: number; y: number; angle: number;
  yawBefore: number; yawNow: number;
  /** No blend: the pose now alone (no pose before, or a jump past `POSE_JUMP_LIMIT`). */
  jump: boolean;
}

// src/sim/pose.ts:122-127 function framesPoseInto
/** The pose between a vehicle's two (`vehicleFramesInto`) at `alpha` of the tick: what `vehiclePoseInto` gives. */
export function framesPoseInto(f: Readonly<VehicleFrames>, alpha: number, out: PoseOut): void;

// src/sim/pose.ts:146-146 variable AXLE_NOW
/** The body centre on the path, pointing along a short chord centred on it. */
/** The frames `axleFrame` writes and reads (`out` of the two in a pose, and the chord's ends): made once. */
const AXLE_NOW = newFrame();

// src/sim/pose.ts:151-162 function axleFrame
function axleFrame(w: SimWorld, vehicle: Vehicle, kinematics: Kinematics, length: number, out: FrameOut): FrameOut | null;
