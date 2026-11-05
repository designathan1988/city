// What the feature uses of src/render/torusWindow.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/torusWindow.ts:4-4 type TexelBand
/** A SQUARE WINDOW OF TEXELS OVER THE UNBOUNDED WORLD, kept in a texture that wraps round (the toroidal update of clipmaps: Tanner, Migdal & Jones, "The Clipmap", SIGGRAPH 1998; Asirvatham & Hoppe, GPU Gems 2 ch. 2): world texel g lives in texel g mod res, so moving the window rewrites only the texels that came in, never the whole texture */

/** A band of world texels, inclusive: columns gx0..gx1, rows gy0..gy1. */
export type TexelBand = readonly [number, number, number, number];
