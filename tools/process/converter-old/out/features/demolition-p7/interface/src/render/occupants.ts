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

// src/render/occupants.ts:50-71 function occupantPlays
/** A seated person's pose at `time` (seconds of their own), written into `out` */
export function occupantPlays(seat: Pick<SeatModel, 'pose' | 'driver' | 'row' | 'z'>, seed: number, time: number, out: Play[], slain = false): Play[];
