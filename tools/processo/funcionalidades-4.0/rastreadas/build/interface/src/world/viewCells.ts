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

// src/world/viewCells.ts:86-93 interface CellEye
/** The eye the view's cells are chosen for: its place on the ground, its motion (world units a second) and its frustum on the plan. */
export interface CellEye {
  readonly x: number;
  readonly y: number;
  readonly vx?: number;
  readonly vy?: number;
  /** Whether a cell's rectangle is in the view; none: every cell is. */
  readonly sees?: (rect: ChangeRect) => boolean;
}

// src/world/viewCells.ts:95-342 class ViewCells
export class ViewCells<V> { … }

// src/world/viewCells.ts:117-119 method ViewCells.cellAt
/** The key of the cell holding a point. */
  cellAt(x: number, y: number): CellKey;

// src/world/viewCells.ts:148-150 method ViewCells.has
/** Whether a cell's product is held (made by this generator). */
  has(key: CellKey): boolean;

// src/world/viewCells.ts:161-170 method ViewCells.ensure
/** A cell's value, derived now if it is not held: what a question about a place far from the view costs (one cell), never the map. */
  ensure(key: CellKey): V;

// src/world/viewCells.ts:186-207 method ViewCells.look
/** The cells the view wants: those within `reach` of the eye and of where its motion takes it, ordered nearest, in view, ahead first. */
  look(eye: CellEye): void;

// src/world/viewCells.ts:242-244 getter ViewCells.wantedKeys
/** The cells the view wants (`look`), in their order. */
  get wantedKeys(): readonly CellKey[];

// src/world/viewCells.ts:247-257 method ViewCells.pump
/** Derives up to `most` of the cells the view wants that are not held, first in the view's order; then keeps to the budgets. The cells derived. */
  pump(most = Infinity): number;

// src/world/viewCells.ts:271-276 method ViewCells.clear
/** Every held cell dropped (a map loaded, a diary that cannot say where). */
  clear(): void;

// src/world/viewCells.ts:279-289 method ViewCells.follow
/** The document's changes since the last look, read off its diary: the cells they reached dropped (everything, when it cannot say where). The cells dropped. */
  follow(journal: ChangeJournal): number;
