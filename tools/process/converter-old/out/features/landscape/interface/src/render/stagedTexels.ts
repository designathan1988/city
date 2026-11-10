// What the feature uses of src/render/stagedTexels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/stagedTexels.ts:6-6 type Texels
/** A DATA TEXTURE'S TEXELS ON THE PAGE ONLY WHILE THREE SENDS THEM: no CPU copy of what the GPU holds (`terrain.ts`) */
// three r186 reads `image.data` only in `WebGLTextures.uploadTexture` (whole, its `layerUpdates`, or its `updateRanges`), then calls `onUpdate`.

type Texels = Float32Array | Uint16Array | Uint8Array;

// src/render/stagedTexels.ts:7-7 type TexelsOf
type TexelsOf<T extends Texels> = { new (length: number): T; readonly name: string };

// src/render/stagedTexels.ts:10-10 interface TexelBox
/** A box of texels, inclusive (column x, row y). */
export interface TexelBox { readonly x0: number; readonly x1: number; readonly y0: number; readonly y1: number }

// src/render/stagedTexels.ts:13-16 interface TexelPart
/** What three is about to send: every texel (`layers` and `box` null), some layers of an array, or a box holding a 2D texture's ranges. */
export interface TexelPart {
  readonly layers: readonly number[] | null;
  readonly box: TexelBox | null;
}

// src/render/stagedTexels.ts:52-59 interface StagedTexels
/** The texture's own array, sent instead of the staging one until it is let go. */
export interface StagedTexels<T extends Texels> {
  /** The array it holds (an edit in progress, a worker's answer), or null. */
  readonly held: T | null;
  /** Holds `array` as its texels; null lets go of the one held. */
  hold(array: T | null): void;
  /** Lets go of the array held once three has sent it (at once when nothing waits to be sent). */
  letGoWhenSent(): void;
}

// src/render/stagedTexels.ts:62-65 interface StageOptions
/** How a texture is staged. */
export interface StageOptions {
  /** false: an array of the texture's own made at each send and let go once sent, for a texture of a size few share (a page-wide one would keep its whole size for good). */
  readonly shared?: boolean;
}

// src/render/stagedTexels.ts:68-116 function stageTexels
/** Makes `texture` read its texels from `fill` at each send, into an array shared by the page (one of its own for the send, `shared` false), unless it holds its own (`hold`). */
export function stageTexels<T extends Texels>(texture: DataTexture | DataArrayTexture, type: TexelsOf<T>, length: number, channels: number,
  fill: (into: T, part: TexelPart) => void, { shared = true }: StageOptions = {}): StagedTexels<T>;
