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

// src/core/fillet.ts:48-48 variable ACUTE_CLEARANCE
/** Small relative clearance added to the acute setback. */
const ACUTE_CLEARANCE = 1.02;

// src/core/fillet.ts:57-57 variable CURB_R_MIN
/** Default curb-return radius for a corner between two half-widths. */
const CURB_R_MIN = m(2.8);

// src/core/fillet.ts:60-60 variable CURB_R_MAX
/** Widest return we will hand out: 7.2 m. */
export const CURB_R_MAX = m(7.2);
