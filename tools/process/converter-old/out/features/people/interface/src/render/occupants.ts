// What the feature uses of src/render/occupants.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/occupants.ts:9-9 type OccupantClip
/** What the people in a vehicle are doing, as clips to play and places to put them */

/** Anything the rigged citizens can play (`riggedCitizens.CitizenClipKey`, without the import cycle). */
type OccupantClip = RiderClipKey | 'walk' | 'walkSlow' | 'standUp' | 'idle';

// src/render/occupants.ts:11-17 interface Play
export interface Play {
  key: OccupantClip;
  phase: number;
  weight: number;
  /** For a walk, ground covered in world units: the stride then plants the feet. */
  distance?: number;
}

// src/render/occupants.ts:20-24 variable SEAT_CLIPS
/** The still poses a seat is sat in: the base, and the two glances blended into it. */
const SEAT_CLIPS: Readonly<Record<SeatPose, { readonly drive: readonly [RiderClipKey, RiderClipKey, RiderClipKey]; readonly ride: readonly [RiderClipKey, RiderClipKey, RiderClipKey] }>> = {
  car: { drive: ['carDrive', 'carDriveMirror', 'carDriveRight'], ride: ['carRide', 'carRideLeft', 'carRideRight'] },
  cab: { drive: ['cabDrive', 'cabDriveMirror', 'cabDriveRight'], ride: ['cabRide', 'cabRideLeft', 'cabRideRight'] },
  chair: { drive: ['chairSit', 'chairSitLeft', 'chairSitRight'], ride: ['chairSit', 'chairSitPhone', 'chairSitRight'] },
};

// src/render/occupants.ts:25-25 variable REAR_CAR_CLIPS
const REAR_CAR_CLIPS = ['carRearRide', 'carRearLeft', 'carRearRight'] as const;

// src/render/occupants.ts:27-29 function clipsFor
const clipsFor = (seat: Pick<SeatModel, 'pose' | 'driver' | 'row'>): readonly [RiderClipKey, RiderClipKey, RiderClipKey] => …;

// src/render/occupants.ts:32-34 function seatBaseClip
/** The pose somebody sits still in, in this seat. */
export function seatBaseClip(seat: Pick<SeatModel, 'pose' | 'driver' | 'row'>): RiderClipKey;

// src/render/occupants.ts:50-71 function occupantPlays
/** A seated person's pose at `time` (seconds of their own), written into `out` */
export function occupantPlays(seat: Pick<SeatModel, 'pose' | 'driver' | 'row' | 'z'>, seed: number, time: number, out: Play[], slain = false): Play[];

// src/render/occupants.ts:74-83 function onePlay
/** DO NOT push a new object a call. */
function onePlay(out: Play[], key: OccupantClip, phase: number): Play[];

// src/render/occupants.ts:97-103 interface KerbPlaces
/** Everything the choreography needs from the body model, in its frame (X forward, Z right). */
export interface KerbPlaces {
  /** The seat's hip point and floor, world units. */
  readonly seat: Pick<SeatModel, 'x' | 'z' | 'hipY' | 'floor' | 'pose' | 'driver' | 'row'>;
  readonly door: Pick<DoorModel, 'side' | 'hingeX' | 'hingeZ' | 'length'>;
  /** Where the person stands on the footway, in the vehicle's frame. */
  readonly foot: { readonly x: number; readonly z: number };
}

// src/render/occupants.ts:106-118 interface KerbFigure
/** One frame of somebody moving between a seat and the footway. */
export interface KerbFigure {
  /** Anchor point in the vehicle frame, world units: see `anchor`. */
  x: number;
  y: number;
  z: number;
  /** `pelvis`: the pelvis bone is on the point. */
  anchor: 'pelvis' | 'feet' | 'feetUnderPelvis';
  /** Heading in the vehicle frame, radians: 0 forward, positive to the left. */
  heading: number;
  /** Whether the seat's size limit applies (inside the car), or the person's own size. */
  seated: boolean;
  readonly plays: Play[];
}

// src/render/occupants.ts:149-222 function kerbFigure
/** Somebody getting out (`drop`) or in (`pick`), at a moment of the transfer */
export function kerbFigure(p: KerbPlaces, kind: 'drop' | 'pick', seated: number, walked: number, f: KerbFigure): KerbFigure;
