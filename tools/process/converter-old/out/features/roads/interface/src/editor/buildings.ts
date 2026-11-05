// What the feature uses of src/editor/buildings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/buildings.ts:76-76 type BuildingContext
/** The building commands. */

export type BuildingContext = SiteContext;

// src/editor/buildings.ts:474-480 function normaliseAngle
export const normaliseAngle = (a: number): number =>;

// src/editor/buildings.ts:576-594 function clearBuildingsOnRoads
/** DO NOT walk `buildings.all()` for an edit */
export function clearBuildingsOnRoads(ctx: BuildingContext, rects: readonly ChangeRect[] | null): number;

// src/editor/buildings.ts:597-601 function clearBuildingsAfterEdit
/** DO NOT look since the last edit: the roads a tile brought in between two edits would demolish or rewrite the buildings they meet. */
export function clearBuildingsAfterEdit(ctx: BuildingContext, before: object): number;
