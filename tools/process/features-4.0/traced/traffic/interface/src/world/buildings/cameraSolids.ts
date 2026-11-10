// What the feature uses of src/world/buildings/cameraSolids.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/cameraSolids.ts:102-102 type Rect
/** A world rectangle: [minX, minY, maxX, maxY]. */
export type Rect = readonly [number, number, number, number];

// src/world/buildings/cameraSolids.ts:105-110 interface GroundSince
/** Where and when the ground the buildings stand on changed (the diary's ground kinds, `render/renderer.ts`). */
export interface GroundSince {
  /** The serial of the latest change of the ground. */
  readonly version: number;
  /** Where the ground changed after `since`; null when the diary cannot say (everywhere). */
  rectsSince(since: number): readonly Rect[] | null;
}

// src/world/buildings/cameraSolids.ts:205-207 constructor SolidFiling.constructor
constructor();
