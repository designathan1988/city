// What the feature uses of src/render/postprocess.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/postprocess.ts:64-72 interface Atmosphere
/** The player's sky (Landscape > Sky and weather) */
export interface Atmosphere {
  /** How much of the sky the old setting covered (0..1). */
  readonly clouds: number;
  readonly cloudBase: number;
  readonly cloudThickness: number;
  /** 0 none (the default: the map seen clear), 1 a thick mist. */
  readonly fog: number;
  readonly fogHeight: number;
}
