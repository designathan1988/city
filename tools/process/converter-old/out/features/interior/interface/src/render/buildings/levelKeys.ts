// What the feature uses of src/render/buildings/levelKeys.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/levelKeys.ts:4-4 variable TILE_CELLS
/** THE KEYS OF THE BUILDINGS' CELLS AND LEVELS: a cell "i,j", a 1,024 m tile "t:x,y", a 2,048 m plate "p:x,y" and the base over a plate's square "b:x,y" (`massBase.ts`) (`massLevels.ts`, `massCells.ts`, `cellPool.ts`); imports nothing, so each reads them without the others */

/** Cells a tile and a plate are on a side (`world/cells.ts` CELL, 256 m: 1,024 m and 2,048 m, `bounds.ts` PLATE_SIZE). */
export const TILE_CELLS = 4;

// src/render/buildings/levelKeys.ts:5-5 variable PLATE_CELLS
const PLATE_CELLS = 8;

// src/render/buildings/levelKeys.ts:7-7 variable PLATE_SHIFT
/** The plates sit half a plate off the cells' origin: the map's middle plate is centred on it (`bounds.ts`); each holds four whole tiles. */
const PLATE_SHIFT = PLATE_CELLS / 2;

// src/render/buildings/levelKeys.ts:9-9 type MassLevel
export type MassLevel = 'tile' | 'plate' | 'base';

// src/render/buildings/levelKeys.ts:24-30 function levelCells
/** The cells a level's key covers: its first column and row and its side, in cells. */
export function levelCells(key: string): { readonly level: MassLevel; readonly i0: number; readonly j0: number; readonly n: number };
