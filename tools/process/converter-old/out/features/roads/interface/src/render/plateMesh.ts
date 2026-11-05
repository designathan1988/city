// What the feature uses of src/render/plateMesh.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/plateMesh.ts:5-16 interface PlateMesh
/** A TERRAIN PLATE'S GROUND FROM ITS CORNERS, off the page (`flatGround.worker.ts`) */
export interface PlateMesh {
  /** The ground at each corner, the terrain's base level added (`TERRAIN_BASE`). */
  readonly heights: Float64Array;
  /** Each cell's diagonal: 1 when it runs a-c (`terrain.ts` chooseDiagonals). */
  readonly flip: Uint8Array;
  /** The land's texels as the patches read them (`terrainLod.ts`): per corner its normal's x and z, its cell's diagonal and its height. */
  readonly texels: Float32Array;
  /** Its levels' nodes: bounds and errors (`LandLevels.data`). */
  readonly levels: Float32Array;
  /** The woods' density but the ecosystem's share, per cell (`woodsBase.ts`), when it was worked out with the ground. */
  woods?: Float32Array;
}
