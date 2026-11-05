// What the feature uses of src/editor/buildings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/buildings.ts:76-76 type BuildingContext
/** The building commands. */

export type BuildingContext = SiteContext;

// src/editor/buildings.ts:78-83 interface EditResult
export interface EditResult {
  readonly ok: boolean;
  readonly problem?: BuildingProblem | 'missing';
  readonly id?: BuildingId;
  readonly volume?: number;
}
