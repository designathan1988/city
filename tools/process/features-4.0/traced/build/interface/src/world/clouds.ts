// What the feature uses of src/world/clouds.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/clouds.ts:8-21 interface PlacedCloud
export interface PlacedCloud {
  readonly id: number;
  /** Map position under the cloud's middle, world units. */
  readonly x: number;
  readonly y: number;
  /** Height of its flat base over the ground's base level, world units. */
  readonly height: number;
  /** Across, world units (a cumulus about twice as wide as tall). */
  readonly size: number;
  /** 0..1: thin and wispy .. a dense white heap. */
  readonly density: number;
  /** Its turn about the vertical, radians: which way its puffs lie. */
  readonly yaw: number;
}
