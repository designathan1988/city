// What the feature uses of src/render/candidateKeep.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/candidateKeep.ts:7-7 variable KEEP_CELL
/** DO NOT keep the whole sweep again on a global key (the roads' revision, the covers' version) */

/** The places' cells, world units: also the cells their drawing is kept in (`plantCells.ts`). */
export const KEEP_CELL = m(64);

// src/render/candidateKeep.ts:10-10 function keepCellKey
/** A cell's key, from its column and row. */
export const keepCellKey = (i: number, j: number): number => …;

// src/render/candidateKeep.ts:13-13 function keepCellOf
/** A key's column and row. */
export const keepCellOf = (key: number): readonly [number, number] => …;
