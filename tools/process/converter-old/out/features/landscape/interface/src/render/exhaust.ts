// What the feature uses of src/render/exhaust.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/exhaust.ts:20-20 type PuffKind
/** 0 exhaust, 1 dust (brown-grey, low and wide), 2 dark smoke (rises), 3 concrete dust (pale, billowing), 4 blood spray */
export type PuffKind = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9;

// src/render/exhaust.ts:22-31 interface Exhaust
export interface Exhaust {
  readonly points: Points;
  /** A vehicle at (x, y) on a deck at height `z`, heading `angle`, `length` long, moving at `speed` (units/s) */
  emit(x: number, y: number, z: number, angle: number, length: number, speed: number, dusty: boolean): void;
  /** `count` puffs of `kind` at once, scattered over `spread` round (x, y, z) */
  burst(x: number, y: number, z: number, count: number, kind: PuffKind, spread: number, size: number, life: number): void;
  /** Advances the clock the shader ages puffs by. */
  tick(seconds: number, halfHeight: number): void;
  dispose(): void;
}

// src/render/exhaust.ts:33-239 function createExhaust
export function createExhaust(): Exhaust;

// src/render/exhaust.ts:219-225 method createExhaust.burst
burst(x, y, z, count, k, spread, s, l);
