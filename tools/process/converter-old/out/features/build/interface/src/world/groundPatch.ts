// What the feature uses of src/world/groundPatch.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/groundPatch.ts:4-6 function cellDiagonal
/** THE DRAWN GROUND OF A PLACE, AS NUMBERS: the land's heights at the corners of the terrain's cells over a rectangle, and the height anywhere inside it read as the terrain mesh reads its own corners (`render/terrain.ts` `sampleGrid`): each cell split along the diagonal with the smaller rise (Garland & Heckbert's data-dependent triangulation), the point on its triangle's plane. What the growth's worker reads the ground through (`editor/growthReplica.ts`): the page samples the ground at a lot's corners of the grid (`sampleGroundPatch`) and sends them with the lot; the worker answers any point of the lot from them (`patchGround`) - the same triangles the ground is drawn with, never a copy of the whole map's ground. Pure: no three, no DOM. */

/** The diagonal a cell is drawn with: 1 the a-c one, 0 the b-d one (a margin keeps near-flat cells on b-d). */
export function cellDiagonal(a: number, b: number, c: number, d: number): 0 | 1;

// src/world/groundPatch.ts:9-12 function cellHeight
/** The height at (u, v) of a cell with corners a (u 0, v 0), b (0, 1), c (1, 1), d (1, 0), split along `diagonal` (`cellDiagonal`). DO NOT change it here alone: the terrain's shaders draw with it (`render/terrainLod.ts` landSurface, landVertex). */
export function cellHeight(a: number, b: number, c: number, d: number, u: number, v: number, diagonal: number): number;

// src/world/groundPatch.ts:15-15 variable ON_LINE
/** How close to a grid line a point counts as on it: the corner itself, not a hair into the next cell. */
const ON_LINE = 1e-9;
