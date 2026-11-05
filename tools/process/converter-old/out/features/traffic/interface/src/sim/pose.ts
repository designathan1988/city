// What the feature uses of src/sim/pose.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/pose.ts:11-14 interface Pose
export interface Pose {
  readonly p: Vec2;
  readonly angle: number;
}

// src/sim/pose.ts:17-20 interface PoseOut
/** A pose written in place (`vehiclePoseInto`): one per caller, made once. */
export interface PoseOut {
  p: { x: number; y: number };
  angle: number;
}

// src/sim/pose.ts:23-25 function newPose
/** A pose to write into. */
export function newPose(): PoseOut;

// src/sim/pose.ts:28-28 variable POSE_JUMP_LIMIT
/** Largest gap, in world units, that two consecutive poses may be apart before the interpolation is abandoned. */
const POSE_JUMP_LIMIT = m(24);

// src/sim/pose.ts:31-33 function laneChangeYaw
/** Angle a vehicle points into its own lane change. */
function laneChangeYaw(slope: number): number;

// src/sim/pose.ts:36-36 variable REAR_AXLE_TO_CENTRE
/** Distance from the rear axle to the middle of the body, as a share of its length */
const REAR_AXLE_TO_CENTRE = 0.3;

// src/sim/pose.ts:39-43 function bodyOffset
/** Sideways offset of the body CENTRE during a lane change. */
function bodyOffset(k: Kinematics, length: number): number;

// src/sim/pose.ts:46-49 function vehiclePose
/** Pose of a vehicle, interpolated between the last two simulation steps. */
export function vehiclePose(w: SimWorld, v: Vehicle, alpha: number): Pose | null;

// src/sim/pose.ts:52-84 function vehiclePoseInto
/** `vehiclePose` into `out`; false where there is none. */
export function vehiclePoseInto(w: SimWorld, v: Vehicle, alpha: number, out: PoseOut): boolean;

// src/sim/pose.ts:87-93 interface VehicleFrames
/** A vehicle's two poses of the tick as `vehiclePoseInto` blends them: the simulation's worker writes these, the page blends. */
export interface VehicleFrames {
  bx: number; by: number; bangle: number;
  x: number; y: number; angle: number;
  yawBefore: number; yawNow: number;
  /** No blend: the pose now alone (no pose before, or a jump past `POSE_JUMP_LIMIT`). */
  jump: boolean;
}

// src/sim/pose.ts:96-119 function vehicleFramesInto
/** `vehiclePoseInto`'s two ends, unblended; false where there is none. Off the road: its free pose, before and now. */
export function vehicleFramesInto(w: SimWorld, v: Vehicle, out: VehicleFrames): boolean;

// src/sim/pose.ts:122-127 function framesPoseInto
/** The pose between a vehicle's two (`vehicleFramesInto`) at `alpha` of the tick: what `vehiclePoseInto` gives. */
export function framesPoseInto(f: Readonly<VehicleFrames>, alpha: number, out: PoseOut): void;

// src/sim/pose.ts:130-130 variable BEFORE_OWN
/** The last step's body centre on its own chart, written in place. */
const BEFORE_OWN = { x: 0, y: 0 };

// src/sim/pose.ts:146-146 variable AXLE_NOW
/** The body centre on the path, pointing along a short chord centred on it. */
/** The frames `axleFrame` writes and reads (`out` of the two in a pose, and the chord's ends): made once. */
const AXLE_NOW = newFrame();

// src/sim/pose.ts:147-147 variable AXLE_BEFORE
const AXLE_BEFORE = newFrame();

// src/sim/pose.ts:148-148 variable AHEAD
const AHEAD = newFrame();

// src/sim/pose.ts:149-149 variable BEHIND
const BEHIND = newFrame();

// src/sim/pose.ts:151-162 function axleFrame
function axleFrame(w: SimWorld, vehicle: Vehicle, kinematics: Kinematics, length: number, out: FrameOut): FrameOut | null;

// src/sim/pose.ts:165-187 function bodyFrame
/** The body's frame `behindFront` behind the front, into `out`; false off the road behind it. */
function bodyFrame(w: SimWorld, vehicle: Vehicle, kinematics: Kinematics, behindFront: number, out: FrameOut): boolean;

// src/sim/pose.ts:190-195 function lerpAngle
/** Interpolates two headings the short way round. */
function lerpAngle(from: number, to: number, t: number): number;
