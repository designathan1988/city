// What the feature uses of src/render/farWoods.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/farWoods.ts:11-19 interface FarWoodsGrid
/** THE WOODS OF A PLATE NOT DRAWN IN FULL (`farForest.ts`) */

/** The terrain's corners per plate side, the cell between them, the plate's half side (`terrain.ts` GRID, TERRAIN_CELL, TERRAIN_HALF). */
export interface FarWoodsGrid {
  readonly side: number;
  readonly cell: number;
  readonly half: number;
  /** The level added to the land (`TERRAIN_BASE`). */
  readonly base: number;
  /** Quads along a side of the far ground's patch of a plate (`flatAtlas.ts` FAR_QUADS): the trees stand on it. */
  readonly farQuads: number;
}

// src/render/farWoods.ts:21-27 interface FarWoodsAsk
export interface FarWoodsAsk {
  readonly relief: ReliefVersion;
  readonly nature: NatureSettings;
  readonly cx: number;
  readonly cy: number;
  readonly grid: FarWoodsGrid;
}

// src/render/farWoods.ts:30-47 interface FarWoods
/** One plate's far trees, as parallel arrays (transferred from the worker, not copied). */
export interface FarWoods {
  readonly count: number;
  /** World position: x east, y north, z up (on the far ground's patch). */
  readonly x: Float32Array;
  readonly y: Float32Array;
  readonly z: Float32Array;
  /** Height, world units. */
  readonly size: Float32Array;
  readonly yaw: Float32Array;
  /** The tree's seed (`natureTrees.ts` seedOfKind): its model and its tint. */
  readonly seed: Float32Array;
  /** Its cell's hash over its band's odds: it stands while this is under the woods' scale. */
  readonly pick: Float32Array;
  /** The plate's sum of the bands' odds over its lattice (the near sweep's `plate.sum`). */
  readonly planned: number;
  /** The lattice's cells read: the work of one plate, the same on any plate of any map. */
  readonly work: number;
}

// src/render/farWoods.ts:80-90 class FarWoodsWork
/** What the far woods work in, plate after plate, owned by the worker that works them (`ecology.worker.ts`): grown, never made per plate (DO NOT make a plate's heights, its ecology's fifteen arrays and its list for one answer). */
export class FarWoodsWork { … }

// src/render/farWoods.ts:93-195 function farWoods
/** Works out one plate's far trees, in `work` (one of its own unless handed). */
export function farWoods(ask: FarWoodsAsk, work = new FarWoodsWork()): FarWoods;
