// What the feature uses of src/world/pointFiling.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/pointFiling.ts:18-18 type PointAt
/** Where a filed id stands, from its record. */
export type PointAt<Id> = (id: Id) => { readonly x: number; readonly y: number };

// src/world/pointFiling.ts:20-113 class PointFiling
export class PointFiling<Id extends number> { … }

// src/world/pointFiling.ts:72-82 method PointFiling.idsMeetingAny
/** The ids standing in any of `rects`, each once, in increasing order (`CellFiling.idsMeetingAny`). */
  idsMeetingAny(rects: readonly ChangeRect[], at: PointAt<Id>): Id[];
