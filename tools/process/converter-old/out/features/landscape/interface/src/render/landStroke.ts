// What the feature uses of src/render/landStroke.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/landStroke.ts:9-9 type StrokeBox
/** WHAT A DAB OF THE BRUSH WORKS OUT AGAIN ON A PLATE, OFF THE PAGE (`landStroke.worker.ts`): no three.js, the page runs the same code when it has no workers */

/** A box of corners, inclusive: x0, x1 across, y0, y1 down (`terrainShaping.ts` TerrainRegion). */
export type StrokeBox = readonly [number, number, number, number];

// src/render/landStroke.ts:12-22 interface LevelsJob
/** The levels of a plate's drawn ground measured again over the corners a dab or a shaping pass moved (`terrainLevels.ts`). */
export interface LevelsJob {
  readonly kind: 'levels';
  /** Cells along a side of the plate. */
  readonly seg: number;
  /** The drawn corners and diagonals as they stand now (copies: the page goes on moving its own). */
  readonly heights: Float64Array;
  readonly flip: Uint8Array;
  /** The levels as the page holds them, every node measured (`LandLevels.data`). */
  readonly levels: Float32Array;
  readonly box: StrokeBox;
}

// src/render/landStroke.ts:25-41 interface WoodsJob
/** The woods' base worked out again over the cells a dab's natural land reaches (`woodsBase.ts` woodsBaseInto). */
export interface WoodsJob {
  readonly kind: 'woods';
  /** The plate's grid: corners along a side, a cell, half its side. */
  readonly n: number;
  readonly cell: number;
  readonly half: number;
  /** Where the plate stands in the world: the woods' noise is the world's. */
  readonly cx: number;
  readonly cy: number;
  /** The natural corners and their diagonals as they stand now (copies). */
  readonly natural: Float64Array;
  readonly flip: Uint8Array;
  /** The plate's own (x, y) the dab moved: min x, min y, max x, max y. */
  readonly rect: readonly [number, number, number, number];
  /** The land at the points off the plate the cells read, in their order (`woodsOutsideReads`), read on the page. */
  readonly outside: Float64Array;
}

// src/render/landStroke.ts:43-43 interface WoodsBlock
/** Some cells of the woods' base: the box of them (inclusive) and their values, row by row. */
export interface WoodsBlock { readonly i0: number; readonly i1: number; readonly j0: number; readonly j1: number; readonly cells: Float32Array }

// src/render/landStroke.ts:46-53 interface ShoreJob
/** The water's level at the corners near water, for the water's mesh made again (`terrain.ts` rebuildWater). */
export interface ShoreJob {
  readonly kind: 'shore';
  /** The water mesh's positions, x, height, z a vertex (three's axes, the plate's own), and how many. */
  readonly position: Float32Array;
  readonly count: number;
  /** The drawn corners the water was made on (a copy). */
  readonly ground: Float64Array;
}

// src/render/landStroke.ts:55-55 type LandStrokeJob
export type LandStrokeJob = LevelsJob | WoodsJob | ShoreJob;

// src/render/landStroke.ts:56-57 type LandStrokeAnswer
export type LandStrokeAnswer<J extends LandStrokeJob> = J extends LevelsJob ? Float32Array : J extends WoodsJob ? WoodsBlock
  : J extends ShoreJob ? Float32Array : never;

// src/render/landStroke.ts:60-60 variable NO_WATER
/** A shore texel with no water near it (`shoreLevelsInto`). */
export const NO_WATER = -100_000;

// src/render/landStroke.ts:62-62 variable GRID
/** Corners per side of a terrain plate. */
const GRID = TERRAIN_SEGMENTS + 1;

// src/render/landStroke.ts:65-96 function shoreLevelsInto
/** The water's level at every terrain corner within a few cells of water, for the shore bands of the terrain shader (`position`: the water mesh's x, height, z a vertex) */
export function shoreLevelsInto(levels: Float32Array, position: ArrayLike<number>, count: number, ground: ArrayLike<number>): void;

// src/render/landStroke.ts:99-105 function measureLevels
/** The levels' nodes after `job.box` moved, as the page's `LandLevels` measures them (`markBox`, `measure`). */
export function measureLevels(job: LevelsJob): Float32Array;

// src/render/landStroke.ts:108-108 interface LandStrokeScratch
/** A whole base the cells are worked out in, kept between jobs by their worker: only the block asked is read back. */
export interface LandStrokeScratch { woods: Float32Array | null }

// src/render/landStroke.ts:111-126 function woodsBlock
/** The cells of the base `job.rect` reaches, as the page works them out (`woodsBaseInto` on the plate's natural land, `terrain.ts` sampleGrid). */
export function woodsBlock(job: WoodsJob, scratch: LandStrokeScratch = { woods: null }): WoodsBlock;

// src/render/landStroke.ts:116-116 function woodsBlock.land
const land = (x: number, y: number): number => …;

// src/render/landStroke.ts:135-147 function runLandStroke
/** A job's answer and the buffers it hands back. */
export function runLandStroke(job: LandStrokeJob, scratch: LandStrokeScratch = { woods: null }): { readonly answer: unknown; readonly transfer: Transferable[] };
