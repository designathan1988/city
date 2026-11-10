// What the feature uses of src/render/signs.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/signs.ts:151-194 function streetChain
/** The segments a street's name runs along: the one under the point, carried straight on through its nodes. */
export function streetChain(net: Network, at: Vec2): SegmentId[];

// src/render/signs.ts:231-231 type SheetCopier
/** Copies a picture onto a sheet's texture on the GPU, its corner at `x`, `y` (three's `copyTextureToTexture`, which makes the sheet's mipmaps again, r186). */
export type SheetCopier = (picture: CanvasTexture, sheet: CanvasTexture, x: number, y: number) => void;
