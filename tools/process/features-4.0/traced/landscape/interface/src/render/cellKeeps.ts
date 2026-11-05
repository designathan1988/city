// What the feature uses of src/render/cellKeeps.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/cellKeeps.ts:7-7 function keepCellAt
/** DO NOT hold a kind's places as one list again: a change in one cell made the whole list, and every place's test, again */

/** The keep cell holding a point. */
export const keepCellAt = (x: number, y: number): number => …;

// src/render/cellKeeps.ts:10-13 function keepCellRect
/** The rectangle a keep cell covers. */
export function keepCellRect(key: number): ChangeRect;

// src/render/cellKeeps.ts:16-21 function keepCellsOver
/** Each keep cell a rectangle grown by `pad` reaches, into `out`. */
export function keepCellsOver(rect: ChangeRect, pad: number, out: Set<number>): Set<number>;

// src/render/cellKeeps.ts:24-106 class CellKeeps
/** The places of a kind kept cell by cell: a `CandidateKeep` per keep cell, filled again on its own */
export class CellKeeps<C extends { … }
