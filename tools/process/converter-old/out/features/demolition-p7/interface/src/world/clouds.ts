// What the feature uses of src/world/clouds.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/clouds.ts:6-6 variable CLOUD_SPAN
/** The sky's period: the placed clouds repeat every this much each way, so over any part of the unbounded world there are clouds (a sky the size of the old map). */
export const CLOUD_SPAN = m(10_240);

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

// src/world/clouds.ts:51-69 function scatterClouds
/** Clouds spread over the map, as a procedural spawner fills a volume and the placed instances are then edited one by one */
export function scatterClouds(count: number, like: { size: number; height: number; density: number }, variation: number,
  half: number, existing: readonly PlacedCloud[], random: () => number): Omit<PlacedCloud, 'id'>[];
