// What the feature uses of src/render/riderPoses.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/riderPoses.ts:5-13 type RiderClipKey
/** Poses for people sitting in and on vehicles, built on the citizens' own skeleton by inverse kinematics. */

export type RiderClipKey =
  | 'carDrive' | 'carDriveMirror' | 'carDriveRight' | 'carDead' | 'cabDead'
  | 'carRide' | 'carRideLeft' | 'carRideRight'
  | 'carRearRide' | 'carRearLeft' | 'carRearRight'
  | 'cabDrive' | 'cabDriveMirror' | 'cabDriveRight'
  | 'cabRide' | 'cabRideLeft' | 'cabRideRight'
  | 'chairSit' | 'chairSitLeft' | 'chairSitRight' | 'chairSitPhone'
  | 'motoRide' | 'motoLeft' | 'motoRight' | 'motoStop'
  | 'bikePedal' | 'bikeLeft' | 'bikeRight' | 'bikeStop';

// src/render/riderPoses.ts:44-44 type V3
type V3 = readonly [number, number, number];

// src/render/riderPoses.ts:47-47 variable SEATED_IDLE
/** Seconds of a seated person's idle loop (`occupants.ts` plays it from each person's own point). */
export const SEATED_IDLE = 6;

// src/render/riderPoses.ts:52-57 interface WheelSpec
/** A steering wheel, from the hip point, metres: its centre ahead and above, the tilt of its rim from vertical */
export interface WheelSpec {
  readonly forward: number;
  readonly up: number;
  readonly tilt: number;
  readonly radius: number;
}

// src/render/riderPoses.ts:60-60 variable DRIVER_WHEEL
/** A car's wheel, per the vehicle spec */
export const DRIVER_WHEEL: WheelSpec = { forward: 0.46, up: 0.36, tilt: 0.45, radius: 0.185 };

// src/render/riderPoses.ts:63-63 variable CAB_WHEEL
/** A bus's or a truck's wheel: large, laid back towards flat */
export const CAB_WHEEL: WheelSpec = { forward: 0.42, up: 0.13, tilt: 1.0, radius: 0.22 };

// src/render/riderPoses.ts:76-96 interface TwoWheelerFit
/** Where a rider sits on a two-wheeler and what they hold and stand on, metres. */
export interface TwoWheelerFit {
  /** Height of the pelvis bone over the road. */
  readonly pelvisY: number;
  /** How far the pelvis bone sits above the saddle's top: the seat of the trousers. */
  readonly saddleDrop: number;
  /** The centre of each grip. */
  readonly grip: { readonly side: number; readonly up: number; readonly forward: number };
  /** The steering axis, ahead of the pelvis: the bars turn about it. */
  readonly steerAxis: number;
  /** A motorcycle's footpegs. */
  readonly peg?: { readonly side: number; readonly up: number; readonly forward: number };
  /** A bicycle's bottom bracket, and its cranks. */
  readonly bracket?: { readonly up: number; readonly forward: number };
  readonly crank?: number;
  /** Pedal centres either side of the frame. */
  readonly pedalSide?: number;
  /** Stopped, the machine tilts this far towards the rider's left foot, which is down. */
  readonly stopTilt: number;
  /** Where that foot is put down: out from the centre line, and a little ahead of the pelvis. */
  readonly footDown: { readonly side: number; readonly forward: number };
}

// src/render/riderPoses.ts:98-106 variable MOTO_FIT
export const MOTO_FIT: TwoWheelerFit = {
  pelvisY: 0.96,
  saddleDrop: 0.1,
  grip: { side: 0.31, up: 0.2, forward: 0.52 },
  steerAxis: 0.8,
  peg: { side: 0.24, up: -0.6, forward: 0.1 },
  stopTilt: 0.14,
  footDown: { side: 0.42, forward: 0.12 },
};

// src/render/riderPoses.ts:108-118 variable BIKE_FIT
export const BIKE_FIT: TwoWheelerFit = {
  pelvisY: 0.98,
  saddleDrop: 0.08,
  grip: { side: 0.27, up: 0.1, forward: 0.52 },
  steerAxis: 0.72,
  bracket: { up: -0.68, forward: 0.22 },
  crank: 0.17,
  pedalSide: 0.13,
  stopTilt: 0.16,
  footDown: { side: 0.34, forward: 0.1 },
};

// src/render/riderPoses.ts:129-136 function gripPoint
/** A grip's centre in the pelvis frame, the bars turned by `steer` (radians, left positive). */
export function gripPoint(fit: TwoWheelerFit, side: 1 | -1, steer = 0): V3;
