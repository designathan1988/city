// What the feature uses of src/render/citizenPlays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenPlays.ts:9-9 variable GPU_PLAYS
/** WHAT A COOKED BODY PLAYS, AS THE GPU'S PALETTE PASS BLENDS IT (`people/crowdAnimation.ts`), AND THE SAME ON THE CPU */

/** Plays the palette pass blends a person (`crowdAnimation.ts` PLAYS): more, and the person is blended on the CPU. */
export const GPU_PLAYS = 4;

// src/render/citizenPlays.ts:12-12 interface Play
/** One play of the pass: an atlas row, the share of the way to the next row, its weight. */
export interface Play { row: number; t: number; weight: number }

// src/render/citizenPlays.ts:15-21 interface CitizenAtlas
/** A body's clips in one atlas texture, and each clip's first row in it. */
export interface CitizenAtlas {
  /** Its texture swapped only when it grows (`appendClip`): the pass reads `anim.atlas`. */
  anim: ClassAnimation;
  readonly rowOf: ReadonlyMap<ClipFrames, number>;
  /** Rows written; the texture's height is its room. */
  rows: number;
}

// src/render/citizenPlays.ts:24-24 variable ATLAS_TEXELS
/** Texels a bone takes in an atlas row (twelve floats: `crowdAnimation.ts` ATLAS_TEXELS). */
const ATLAS_TEXELS = 3;

// src/render/citizenPlays.ts:29-36 function atlasTexture
/** An atlas texture of `height` rows, its texels `pixels`. */
function atlasTexture(pixels: Float32Array, width: number, height: number): DataTexture;

// src/render/citizenPlays.ts:39-41 function animationOf
/** The atlas as the palette pass reads it (`ClassAnimation`). */
function animationOf(texture: DataTexture, bones: number, width: number, height: number): ClassAnimation;
