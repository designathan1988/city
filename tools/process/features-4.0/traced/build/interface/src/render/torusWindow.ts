// What the feature uses of src/render/torusWindow.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/torusWindow.ts:4-4 type TexelBand
/** A SQUARE WINDOW OF TEXELS OVER THE UNBOUNDED WORLD, kept in a texture that wraps round (the toroidal update of clipmaps: Tanner, Migdal & Jones, "The Clipmap", SIGGRAPH 1998; Asirvatham & Hoppe, GPU Gems 2 ch. 2): world texel g lives in texel g mod res, so moving the window rewrites only the texels that came in, never the whole texture */

/** A band of world texels, inclusive: columns gx0..gx1, rows gy0..gy1. */
export type TexelBand = readonly [number, number, number, number];

// src/render/torusWindow.ts:6-62 class TorusWindow
export class TorusWindow { … }

// src/render/torusWindow.ts:8-8 field TorusWindow.ox
/** The world texel of the window's low corner (west, south). */
  ox: number;

// src/render/torusWindow.ts:9-9 field TorusWindow.oy
oy: number;

// src/render/torusWindow.ts:11-14 constructor TorusWindow.constructor
constructor(readonly res: number, readonly cell: number, cx = 0, cy = 0);

// src/render/torusWindow.ts:17-19 getter TorusWindow.span
/** The window's side, world units: the shader reads world / span with the texture repeating. */
  get span(): number;

// src/render/torusWindow.ts:22-24 method TorusWindow.texel
/** The texture's column (or row) of world texel `g`. */
  texel(g: number): number;

// src/render/torusWindow.ts:46-61 method TorusWindow.follow
/** Centres the window on (x, y) once it strayed more than `hold` texels: the bands of texels that came in (to write), or null when it stays. */
  follow(x: number, y: number, hold: number): TexelBand[] | null;
