// What the feature uses of src/render/people/crowdAnimation.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/crowdAnimation.ts:36-46 interface ClassAnimation
export interface ClassAnimation {
  /** Frames of every clip, a row each: `bones * 3` texels across. */
  readonly atlas: DataTexture;
  /** A clip's first row in the atlas. */
  rowOf(clip: string): number;
  readonly bones: number;
  /** The atlas's bytes on the GPU. */
  readonly bytes: number;
  /** The atlas sent up again, its texels made from the clips (a WebGL context restored); only for a `gpuOnly` atlas. */
  refill(): void;
}

// src/render/people/crowdAnimation.ts:87-91 interface PalettePlay
/** One play of a person in the palette pass: an atlas row, the fraction to the next row, its weight. */
interface PalettePlay {
  readonly row: number;
  readonly t: number;
  readonly weight: number;
}

// src/render/people/crowdAnimation.ts:93-113 interface PalettePass
export interface PalettePass {
  /** The bone palettes, a row a person (`bones * 4` texels: mat4 columns), for the skinning to read: a target shared with other passes (`paletteTargets.ts`), this pass's rows from `base`. */
  readonly texture: Texture;
  readonly base: number;
  readonly capacity: number;
  /** Settles once its program is built (`uploads.ts` compilePassAhead): nobody draws with its palettes before. */
  readonly ready: Promise<void>;
  /** What person `row` plays this frame, the body variant whose joints they have; null plays: a palette of their own (`setOverride`). */
  setPlays(row: number, plays: readonly PalettePlay[] | null, variant: number): void;
  /** Person `row` is not drawn this frame: their row is not blended. */
  rest(row: number): void;
  /** A palette worked out on the CPU for a person (held by a ragdoll, wounded, a limb lost). */
  setOverride(row: number, palette: Float32Array): void;
  /** Rows 0..`rows` worked out on the GPU. */
  render(renderer: WebGLRenderer, rows: number): void;
  /** Room for `capacity` rows; the rows written so far are kept. */
  grow(capacity: number): void;
  /** The clips' atlas read from now on (a grown one, `citizenPlays.ts` appendClip): the same program and targets. */
  setAtlas(atlas: Texture): void;
  dispose(): void;
}
