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

// src/render/plateMesh.ts:19-70 function landNormalsInto
/** Each corner's normal in a box of corners (inclusive), area-weighted over the faces round it as three's `computeVertexNormals` forms them, into the land's texels (x, z). */
export function landNormalsInto(
  heights: ArrayLike<number>,
  flip: ArrayLike<number>,
  side: number,
  half: number,
  cell: number,
  texels: Float32Array,
  x0: number,
  x1: number,
  y0: number,
  y1: number,
): void;

// src/render/plateMesh.ts:73-98 function buildPlateMesh
/** Builds a plate's ground from its corners (`ground[j * side + i]`, without the base level). */
export function buildPlateMesh(ground: Float64Array, side: number, half: number, cell: number, base: number): PlateMesh;
