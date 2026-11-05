// What the feature uses of src/render/ragdollGround.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/ragdollGround.ts:10-19 interface GroundCorners
export interface GroundCorners {
  /** Height at world (x, y): between its cell's four corners, as a physics engine's heightfield is between its samples. */
  at(x: number, y: number): number;
  /** Lets go of the tiles nobody read since the last sweep (once a frame): what still moves on the ground keeps its own. */
  sweep(): void;
  /** Every height found forgotten: the ground under them changed. */
  forget(): void;
  /** Tiles held now (tests, probes). */
  readonly tiles: number;
}

// src/render/ragdollGround.ts:24-72 function createGroundCorners
/** DO NOT cap it and clear it whole: a cap thrown away mid-fall is found again corner by corner, every body at once */
export function createGroundCorners(corner: (x: number, y: number) => number): GroundCorners;
