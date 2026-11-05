// What the feature uses of src/world/viewCells.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/viewCells.ts:23-23 type CellKey
/**
 * THE VIEW'S CELLS: WHAT IS DERIVED FROM THE DOCUMENT, HELD ONLY ROUND THE CAMERA
 *
 * The document stays whole in memory; what a system works out of it (a cover index, the road surfaces' rings) is held cell by cell on
 * the world's fixed grid (`cells.ts`), each cell at a stage: nothing (the document only), derived (its product on the CPU), resident
 * (its product holding GPU bytes too). A product is a pure function of the cell and the document (`derive`), stamped with its
 * generator's version: it can be made again at any moment, so what is far from the camera is let go and made again when wanted.
 *
 * - A view scheduler (`look`, `pump`) asks the cells round the eye nearest first, those in the frustum before those behind, and the
 *   ones ahead of the eye's motion before it gets there (Cesium's `foveatedScreenSpaceError`, Minecraft's ticket levels).
 * - Two separate LRU budgets, one of CPU bytes and one of GPU bytes (Cesium's `cacheBytes` bounds the GPU only, and its CPU tree grew
 *   without bound: both are bounded here). A cell the view wants is never let go for the budget; past it the least recently used go.
 * - The document's diary (`follow`) drops the cells an edit reached, nothing else: an edit costs the cells it touched, not the map.
 *
 * DO NOT keep a product for every cell ever derived: a cell is held by the view or by the budget, never by having been asked once.
 */

/** A cell's key: its column and row on the world's grid (`cells.ts` `cellAt`). */
export type CellKey = number;

// src/world/viewCells.ts:26-26 variable CellStage
/** A cell's stage. */
export const CellStage = { none: 0, derived: 1, resident: 2 } as const;

// src/world/viewCells.ts:27-27 type CellStage
export type CellStage = (typeof CellStage)[keyof typeof CellStage];

// src/world/viewCells.ts:95-342 class ViewCells
export class ViewCells<V> { … }

// src/world/viewCells.ts:292-314 method ViewCells.trim
/** Keeps the products to the budgets: the least recently used out first, never a cell the view wants nor one of `keep`. `cpuBudget`: a lower one to rest at (after a build swept the map). */
  trim(keep: ReadonlySet<CellKey> | null, cpuBudget = this.spec.cpuBudget): void;
