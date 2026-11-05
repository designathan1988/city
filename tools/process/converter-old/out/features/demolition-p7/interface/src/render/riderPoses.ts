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

// src/render/riderPoses.ts:15-22 interface RiderClip
export interface RiderClip {
  readonly key: RiderClipKey;
  /** Seconds of the baked clip. A pedalling clip is one crank revolution. */
  readonly duration: number;
  readonly loop: boolean;
  /** Poses the rig at `time`; the rig starts from its rest pose each frame. */
  pose(rig: Object3D, time: number): void;
}

// src/render/riderPoses.ts:25-42 interface Targets
/** Targets for one pose, in metres in the pelvis frame. */
interface Targets {
  /** Forward lean of the trunk, radians (negative reclines). */
  readonly lean: number;
  /** Twist of the trunk towards the left, radians. */
  readonly twist?: number;
  readonly leftFoot: V3;
  readonly rightFoot: V3;
  readonly leftHand: V3;
  readonly rightHand: V3;
  /** Pole the knees bend towards (forward and up for a seated person). */
  readonly kneePole?: V3;
  /** Pole the elbows bend towards (down and out). */
  readonly elbowPole?: V3;
  /** Head pitch to keep the eyes on the road, radians (positive looks down). */
  readonly look?: number;
  /** Head turn, radians, positive to the left; shared between the neck and the head. */
  readonly turn?: number;
}

// src/render/riderPoses.ts:44-44 type V3
type V3 = readonly [number, number, number];

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

// src/render/riderPoses.ts:66-71 function rimPoint
/** A point on the rim of a wheel, pelvis frame (+X left, +Y up, +Z forward) */
function rimPoint(clock: number, wheel: WheelSpec = DRIVER_WHEEL): V3;

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

// src/render/riderPoses.ts:126-126 variable ANKLE_OVER_PEDAL
/** Where the ankle is over a peg or a pedal under the ball of the foot. */
const ANKLE_OVER_PEDAL: V3 = [0, 0.07, -0.08];

// src/render/riderPoses.ts:138-138 function offset
const offset = (p: V3, d: V3, side: 1 | -1): V3 => …;

// src/render/riderPoses.ts:148-154 function footDownTarget
/** The ankle target for a foot put down on the road beside a machine tilted by `fit.stopTilt` */
function footDownTarget(fit: TwoWheelerFit): V3;

// src/render/riderPoses.ts:156-161 function hands
function hands(fit: TwoWheelerFit, steer = 0): Pick<Targets, 'leftHand' | 'rightHand'>;

// src/render/riderPoses.ts:166-177 variable CAR_DRIVE
/** A car seat: reclined a little, thighs near level, feet forward on the pedals or the floor */
const CAR_DRIVE: Targets = {
  // The rest pose already stoops about 0.2 rad forward
  lean: -0.45,
  // Heels on the floor (the ankle 8 cm over it, the hip point 0.28 m up) and 0.8 m ahead of the hip, on the pedals.
  leftFoot: [0.17, -0.08, 0.62],
  rightFoot: [-0.14, -0.08, 0.64],
  leftHand: rimPoint(1.05),
  rightHand: rimPoint(-1.05),
  kneePole: [0, 1, 1],
  elbowPole: [0, -1, 0.2],
  look: -0.12,
};

// src/render/riderPoses.ts:178-188 variable CAR_RIDE
const CAR_RIDE: Targets = {
  lean: -0.45,
  // Legs out, the knees a little over the hips
  leftFoot: [0.16, -0.08, 0.61],
  rightFoot: [-0.16, -0.08, 0.61],
  leftHand: [0.13, 0.06, 0.3],
  rightHand: [-0.13, 0.06, 0.3],
  kneePole: [0, 1, 1],
  elbowPole: [0.3, -1, 0],
  look: -0.1,
};

// src/render/riderPoses.ts:191-198 variable CAR_REAR_RIDE
/** A rear passenger folds their knees into the footwell beneath the front seat. */
const CAR_REAR_RIDE: Targets = {
  ...CAR_RIDE,
  lean: -0.38,
  leftFoot: [0.15, -0.13, 0.43],
  rightFoot: [-0.15, -0.13, 0.44],
  leftHand: [0.14, 0.04, 0.23],
  rightHand: [-0.14, 0.04, 0.23],
};

// src/render/riderPoses.ts:201-210 variable CAB_DRIVE
/** The driver's seat of a bus or a truck cab */
const CAB_DRIVE: Targets = {
  lean: -0.42,
  leftFoot: [0.16, -0.37, 0.4],
  rightFoot: [-0.15, -0.37, 0.43],
  leftHand: rimPoint(1.15, CAB_WHEEL),
  rightHand: rimPoint(-1.15, CAB_WHEEL),
  kneePole: [0, 1, 1],
  elbowPole: [0, -1, 0.1],
  look: -0.05,
};

// src/render/riderPoses.ts:211-220 variable CAB_RIDE
const CAB_RIDE: Targets = {
  lean: -0.42,
  leftFoot: [0.16, -0.37, 0.38],
  rightFoot: [-0.16, -0.37, 0.38],
  leftHand: [0.13, 0.02, 0.27],
  rightHand: [-0.13, 0.02, 0.27],
  kneePole: [0, 1, 1],
  elbowPole: [0.3, -1, 0],
  look: 0,
};

// src/render/riderPoses.ts:223-232 variable CHAIR_SIT
/** A bus seat: upright, thighs level, shins down to the floor 0.55 m below the hip, hands resting on the thighs. */
const CHAIR_SIT: Targets = {
  lean: -0.3,
  leftFoot: [0.12, -0.46, 0.4],
  rightFoot: [-0.12, -0.46, 0.4],
  leftHand: [0.12, 0.03, 0.27],
  rightHand: [-0.12, 0.03, 0.27],
  kneePole: [0, 1, 1],
  elbowPole: [0.3, -1, 0],
  look: 0.05,
};

// src/render/riderPoses.ts:235-242 function dead
/** A driver shot dead at the wheel, as GTA V draws one */
const dead = (t: Targets, lean: number): Targets => …;

// src/render/riderPoses.ts:245-245 function turned
/** A head turned, and the shoulders a little with it. */
const turned = (t: Targets, turn: number, look = t.look ?? 0): Targets => …;

// src/render/riderPoses.ts:248-256 variable MOTO
/** Astride a motorcycle: leaning into the tank, hands on the grips, feet on the pegs (`MOTO_FIT`). */
const MOTO: Targets = {
  lean: 0.28,
  ...hands(MOTO_FIT),
  leftFoot: offset([MOTO_FIT.peg!.side, MOTO_FIT.peg!.up, MOTO_FIT.peg!.forward], ANKLE_OVER_PEDAL, 1),
  rightFoot: offset([-MOTO_FIT.peg!.side, MOTO_FIT.peg!.up, MOTO_FIT.peg!.forward], ANKLE_OVER_PEDAL, -1),
  kneePole: [0.5, 0.1, 1],
  elbowPole: [0.6, -1, 0],
  look: -0.12,
};

// src/render/riderPoses.ts:258-258 variable MOTO_STOP
/** Stopped: the machine tilted onto the left foot, the right on its peg. */
const MOTO_STOP: Targets = { ...MOTO, lean: 0.12, leftFoot: footDownTarget(MOTO_FIT) };

// src/render/riderPoses.ts:261-267 variable BIKE
/** On a bicycle: a forward reach to the bars and the feet on the pedals, which turn round the bottom bracket (`BIKE_FIT`). */
const BIKE: Omit<Targets, 'leftFoot' | 'rightFoot'> = {
  lean: 0.32,
  ...hands(BIKE_FIT),
  kneePole: [0.1, 0.2, 1],
  elbowPole: [0.4, -1, 0],
  look: -0.2,
};

// src/render/riderPoses.ts:268-268 function pedalFoot
const pedalFoot = (angle: number, side: 1 | -1): V3 => …;

// src/render/riderPoses.ts:271-273 function steered
/** Steering: the bars turned by `STEER_FULL`, the hands with them, the trunk following a little. */
function steered<T extends Omit<Targets, 'leftFoot' | 'rightFoot'>>(t: T, fit: TwoWheelerFit, direction: 1 | -1): T;

// src/render/riderPoses.ts:275-314 variable RIDER_CLIPS
export const RIDER_CLIPS: readonly RiderClip[] = [
  seated('carDrive', CAR_DRIVE),
  seated('carDriveMirror', turned(CAR_DRIVE, 0.45, -0.05)),
  seated('carDriveRight', turned(CAR_DRIVE, -0.45)),
  still('carDead', dead(CAR_DRIVE, 0.4)),
  still('cabDead', dead(CAB_DRIVE, 0.45)),
  seated('carRide', CAR_RIDE),
  seated('carRideLeft', turned(CAR_RIDE, 0.45)),
  seated('carRideRight', turned(CAR_RIDE, -0.45)),
  seated('carRearRide', CAR_REAR_RIDE),
  seated('carRearLeft', turned(CAR_REAR_RIDE, 0.45)),
  seated('carRearRight', turned(CAR_REAR_RIDE, -0.45)),
  seated('cabDrive', CAB_DRIVE),
  seated('cabDriveMirror', turned(CAB_DRIVE, 0.6, 0)),
  seated('cabDriveRight', turned(CAB_DRIVE, -0.5, 0)),
  seated('cabRide', CAB_RIDE),
  seated('cabRideLeft', turned(CAB_RIDE, 0.55)),
  seated('cabRideRight', turned(CAB_RIDE, -0.6)),
  seated('chairSit', CHAIR_SIT),
  seated('chairSitLeft', turned(CHAIR_SIT, 0.65)),
  seated('chairSitRight', turned(CHAIR_SIT, -0.65)),
  seated('chairSitPhone', { ...CHAIR_SIT, leftHand: [0.05, 0.14, 0.3], rightHand: [-0.05, 0.14, 0.3], elbowPole: [0.6, -1, -0.2], look: 0.55 }),
  still('motoRide', MOTO),
  still('motoLeft', steered(MOTO, MOTO_FIT, 1)),
  still('motoRight', steered(MOTO, MOTO_FIT, -1)),
  still('motoStop', MOTO_STOP),
  {
    key: 'bikePedal',
    duration: 1,
    loop: true,
    pose(rig, time) {
      const angle = time * Math.PI * 2;
      apply(rig, { ...BIKE, leftFoot: pedalFoot(angle, 1), rightFoot: pedalFoot(angle + Math.PI, -1) });
    },
  },
  still('bikeLeft', { ...steered(BIKE, BIKE_FIT, 1), leftFoot: pedalFoot(0, 1), rightFoot: pedalFoot(Math.PI, -1) }),
  still('bikeRight', { ...steered(BIKE, BIKE_FIT, -1), leftFoot: pedalFoot(0, 1), rightFoot: pedalFoot(Math.PI, -1) }),
  // Stopped: tilted onto the left foot, the right on its pedal at the top of the stroke, ready to push off.
  still('bikeStop', { ...BIKE, lean: 0.18, leftFoot: footDownTarget(BIKE_FIT), rightFoot: pedalFoot(Math.PI * 0.6, -1) }),
];

// src/render/riderPoses.ts:319-325 variable NO_HELMET
/** Bodies whose hair no helmet of a helmet's size holds - buns, big curls, a ponytail high on the crown */
export const NO_HELMET: ReadonlySet<string> = new Set([
  'female_01', 'female_02', 'female_04', 'female_07', 'female_09', 'female_11', 'female_12', 'female_17',
  'female_party_02', 'medical_female_01', 'medical_female_03', 'medical_male_04', 'pilot_male_03',
  'police_male_01', 'security_female_01', 'sports_male_03',
  // Long hair a helmet would sit on, not round.
  'female_15',
]);

// src/render/riderPoses.ts:380-382 function still
/** Seconds a still pose is baked over: two frames, the same pose. */
function still(key: RiderClipKey, targets: Targets): RiderClip;

// src/render/riderPoses.ts:385-406 function seated
/** A seated pose that lives: breathing in the trunk, the head turning a little and nodding */
function seated(key: RiderClipKey, targets: Targets): RiderClip;

// src/render/riderPoses.ts:479-517 function apply
/** Poses the rig so its limbs reach the targets. The rig must be at its rest pose. */
function apply(rig: Object3D, t: Targets): void;
