// What the feature uses of src/world/fogPaint.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/fogPaint.ts:4-16 interface FogDab
/** PAINTED FOG: banks of mist the player lays where they want them - over a valley, along a river, round a hill */

export interface FogDab {
  readonly x: number;
  readonly y: number;
  readonly radius: number;
  /** 0..1: how far one dab takes the fog towards full (or, erasing, towards none) at its centre. */
  readonly strength: number;
  /** How high this fog stands over the ground before it thins out, world units. */
  readonly height: number;
  /** How fast the wind carries this fog through, world units a second. */
  readonly speed: number;
  /** Takes fog away instead of laying it. */
  readonly erase?: boolean;
}

// src/world/fogPaint.ts:59-65 interface FogSample
export interface FogSample {
  /** 0..1. */
  density: number;
  /** The fog's height and speed there: the dabs that laid it, weighted by what each laid. */
  height: number;
  speed: number;
}

// src/world/fogPaint.ts:68-88 function fogAt
/** The fog the dabs leave at a point: each dab in turn, a smooth falloff from its centre to its rim, adding towards one or */
export function fogAt(dabs: readonly FogDab[], x: number, y: number, out: FogSample = { density: 0, height: 0, speed: 0 }): FogSample;
