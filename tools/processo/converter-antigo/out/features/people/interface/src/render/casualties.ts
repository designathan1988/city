// What the feature uses of src/render/casualties.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/casualties.ts:15-26 interface BloodDecal
/** A patch of blood on the ground: where, how big at full spread, and how long it takes to spread. */
export interface BloodDecal {
  readonly x: number;
  readonly y: number;
  /** World height of the surface it lies on. */
  readonly z: number;
  readonly angle: number;
  readonly size: number;
  /** Seconds to reach full size (0: at once). */
  readonly spread: number;
  /** Seconds since it was made. */
  age: number;
}
