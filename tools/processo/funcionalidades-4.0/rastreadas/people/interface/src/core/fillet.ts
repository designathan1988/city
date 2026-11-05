// What the feature uses of src/core/fillet.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/fillet.ts:5-16 interface Fillet
export interface Fillet {
  /** Tangency point on the `dirA` side. */
  readonly ta: Vec2;
  /** Tangency point on the `dirB` side. */
  readonly tb: Vec2;
  /** Arc centre. */
  readonly c: Vec2;
  /** Arc radius. */
  readonly r: number;
  /** Distance from the corner to each tangency. */
  readonly run: number;
}

// src/core/fillet.ts:19-45 function filletCorner
/** Rounds the corner at `x` where the boundary leaves along `dirA` on one side and `dirB` on the other. */
export function filletCorner(
  x: Vec2,
  dirA: Vec2,
  dirB: Vec2,
  r: number,
): Fillet | null;
