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
