// What the feature uses of src/render/citizenPlays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/citizenPlays.ts:15-21 interface CitizenAtlas
/** A body's clips in one atlas texture, and each clip's first row in it. */
export interface CitizenAtlas {
  /** Its texture swapped only when it grows (`appendClip`): the pass reads `anim.atlas`. */
  anim: ClassAnimation;
  readonly rowOf: ReadonlyMap<ClipFrames, number>;
  /** Rows written; the texture's height is its room. */
  rows: number;
}
