// What the feature uses of src/render/cellKeeps.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/cellKeeps.ts:16-21 function keepCellsOver
/** Each keep cell a rectangle grown by `pad` reaches, into `out`. */
export function keepCellsOver(rect: ChangeRect, pad: number, out: Set<number>): Set<number>;

// src/render/cellKeeps.ts:24-106 class CellKeeps
/** The places of a kind kept cell by cell: a `CandidateKeep` per keep cell, filled again on its own */
export class CellKeeps<C extends { … }
