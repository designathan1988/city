// What the feature uses of src/render/fogLayer.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/fogLayer.ts:9-9 variable FOG_RES
/** The painted fog as the post pass reads it (`postprocess.ts` CLOUD_SHADOWS): a window of the land round the view, wrapping round (`torusWindow.ts`) */

/** Texels across the window: 20 m a texel over its 10,240 m (the old map's, so the same fog where it was). */
const FOG_RES = 512;

// src/render/fogLayer.ts:10-10 variable FOG_CELL
const FOG_CELL = m(20);

// src/render/fogLayer.ts:12-12 variable FOG_HOLD
/** How far the view goes from the window's middle before it follows (texels): the texels that came in written, no others. */
const FOG_HOLD = FOG_RES / 8;

// src/render/fogLayer.ts:14-14 variable FOG_BLOCK
/** Texels a block of a row: each block's slab from its half floats kept, read back only when texels of it were written (TER-14). */
const FOG_BLOCK = 64;

// src/render/fogLayer.ts:15-15 variable FOG_BLOCKS
const FOG_BLOCKS = FOG_RES / FOG_BLOCK;

// src/render/fogLayer.ts:17-26 interface FogLayer
export interface FogLayer {
  readonly texture: DataTexture;
  /** Lowest ground under any fog and the highest its top reaches, world units (the slab the rays march). */
  readonly low: number;
  readonly high: number;
  /** Any fog at all. */
  readonly any: boolean;
  /** The window's low corner (world x, y) and its side: the shader reads world / side, the texture repeating, nothing off it. */
  readonly window: readonly [number, number, number];
}

// src/render/fogLayer.ts:40-40 type Rect
/** A world rectangle: [minX, minY, maxX, maxY]. */
type Rect = readonly [number, number, number, number];

// src/render/fogLayer.ts:43-50 interface FogField
/** DO NOT raster the whole window at every dab: 262 144 texels and a ground height each, for a brush's few metres. */
export interface FogField {
  /** Every texel, from these dabs and this ground (the first time, or when the diary cannot say where). */
  whole(dabs: readonly FogDab[], groundAt: (x: number, y: number) => number): FogLayer;
  /** Only the texels inside `rects`. */
  update(dabs: readonly FogDab[], groundAt: (x: number, y: number) => number, rects: readonly Rect[]): FogLayer;
  /** The window kept round where the view looks (world x, y): only the texels that came in written; null when it stayed. */
  follow(x: number, y: number, dabs: readonly FogDab[], groundAt: (x: number, y: number) => number): FogLayer | null;
}

// src/render/fogLayer.ts:52-204 function createFogField
export function createFogField(texture: DataTexture): FogField;
