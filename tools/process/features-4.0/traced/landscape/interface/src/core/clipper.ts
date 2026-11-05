// What the feature uses of src/core/clipper.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/clipper.ts:6-6 type Ring
/** A ring need not repeat its first point or use a particular winding. */
export type Ring = number[][];

// src/core/clipper.ts:8-8 type Poly
/** The first ring is the exterior; subsequent rings are holes. */
export type Poly = Ring[];

// src/core/clipper.ts:9-9 type MultiPoly
export type MultiPoly = Poly[];
