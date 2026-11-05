// What the feature uses of src/world/roads/buildMode.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/buildMode.ts:10-10 type BuildMode
/** HOW A ROAD IS BUILT at each point along it */
export type BuildMode = 'ground' | 'embankment' | 'cutting' | 'bridge' | 'tunnel';

// src/world/roads/buildMode.ts:24-34 interface RoadStation
/** A point of a road being built: where, how far along, its deck, the ground under it, and how. */
export interface RoadStation {
  /** Distance along the road from its start (world units). */
  readonly s: number;
  readonly x: number;
  readonly y: number;
  /** The solved deck height (world height). */
  readonly deck: number;
  /** The natural ground under it. */
  readonly ground: number;
  readonly mode: BuildMode;
}

// src/world/roads/buildMode.ts:37-41 interface BuildRun
/** A stretch built one way. */
export interface BuildRun {
  readonly mode: BuildMode;
  readonly from: number;
  readonly to: number;
}

// src/world/roads/buildMode.ts:110-124 function buildRuns
/** The stations as stretches of one way of building, in order. */
export function buildRuns(stations: readonly RoadStation[]): BuildRun[];
