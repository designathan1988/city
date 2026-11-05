// What the feature uses of src/world/cells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/cells.ts:9-9 variable CELL
/** THE WORLD'S CELLS: one fixed grid over the plane the map is laid on, the partition an edit's cost is measured in */

/** The side of a cell, world units (Unreal's default runtime grid cell). */
export const CELL = m(256);

// src/world/cells.ts:60-255 class CellFiling
/** Things (by id) filed in the cells their bounds reach, each with the object it was filed for */
export class CellFiling<Id extends number, T extends object> { … }

// src/world/cells.ts:131-154 method CellFiling.idsMeeting
/** The ids whose bounds meet `rect`, each once, in increasing order */
  idsMeeting(rect: ChangeRect): Id[];
