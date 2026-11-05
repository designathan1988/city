// What the feature uses of src/core/clipper.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/clipper.ts:6-6 type Ring
/** A ring need not repeat its first point or use a particular winding. */
export type Ring = number[][];

// src/core/clipper.ts:8-8 type Poly
/** The first ring is the exterior; subsequent rings are holes. */
export type Poly = Ring[];

// src/core/clipper.ts:9-9 type MultiPoly
export type MultiPoly = Poly[];

// src/core/clipper.ts:14-14 variable CLIPPER_SCALE
/** The grid Clipper's input is rounded to, steps per world unit (`input`): what a test against its result must round to too. */
export const CLIPPER_SCALE = SCALE;

// src/core/clipper.ts:143-146 function union
/** Boolean set operations preserve outer/hole membership through PolyTree. */
export function union(a: MultiPoly, b: MultiPoly = []): MultiPoly;
