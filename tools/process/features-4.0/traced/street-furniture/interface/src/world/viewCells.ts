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

// src/world/viewCells.ts:30-35 interface CellProduct
/** What a cell's derivation made: its value, and the bytes it holds on the CPU and on the GPU. */
export interface CellProduct<V> {
  readonly value: V;
  readonly cpuBytes: number;
  /** Bytes on the GPU: a product holding some is resident. */
  readonly gpuBytes?: number;
}

// src/world/viewCells.ts:37-59 interface ViewCellsSpec
export interface ViewCellsSpec<V> {
  /** For the ledgers and the console. */
  readonly name: string;
  /** The generator's version: a product made by another version is made again. */
  readonly version: number;
  /** The side of the cells, world units: the world's grid by default. */
  readonly side?: number;
  /** How far round the eye the cells are kept derived, world units. */
  readonly reach: number;
  /** The most the products hold on the CPU, bytes (the cells the view wants are kept past it). */
  readonly cpuBudget: number;
  /** The most the products hold on the GPU, bytes. */
  readonly gpuBudget?: number;
  /** The kinds of the document's changes a product reads (`follow`), and how far past a change's rectangle a product may read. */
  readonly kinds?: readonly ChangeKind[];
  readonly pad?: number;
  /** The product of a cell, from the document as it is: a pure function of the cell (and the document). */
  derive(key: CellKey, rect: ChangeRect): CellProduct<V>;
  /** A product let go (out of the budget, or made stale): what it holds elsewhere given back. */
  release?(key: CellKey, value: V): void;
  /** A resident product's GPU share let go, the cell kept derived: its GPU bytes are 0 after it. None: the whole product goes. */
  releaseGpu?(key: CellKey, value: V): void;
}

// src/world/viewCells.ts:62-66 interface ViewCellsWork
/** What the cells did since they were made: the products derived, those let go for the budget, those dropped by edits. */
export interface ViewCellsWork {
  derived: number;
  evicted: number;
  invalidated: number;
}

// src/world/viewCells.ts:68-72 interface Held
interface Held<V> {
  product: CellProduct<V>;
  version: number;
  stage: CellStage;
}

// src/world/viewCells.ts:95-342 class ViewCells
export class ViewCells<V> { … }

// src/world/viewCells.ts:98-98 field ViewCells.held
private readonly held = new Map<CellKey, Held<V>>();

// src/world/viewCells.ts:102-102 field ViewCells.wanted
/** The cells the view wants, in the order they are asked (nearest, in view, ahead first). */
  private wanted: CellKey[] = [];

// src/world/viewCells.ts:103-103 field ViewCells.wantedSet
private readonly wantedSet = new Set<CellKey>();

// src/world/viewCells.ts:105-105 field ViewCells.cpuBytes
/** Bytes the held products hold now. */
  cpuBytes = 0;

// src/world/viewCells.ts:106-106 field ViewCells.gpuBytes
gpuBytes = 0;

// src/world/viewCells.ts:107-107 field ViewCells.work
readonly work: ViewCellsWork = { derived: 0, evicted: 0, invalidated: 0 };

// src/world/viewCells.ts:109-109 field ViewCells.version
/** Moves whenever a held product is dropped for a change of the document (`invalidate`): what the products answer may differ since. */
  version = 0;

// src/world/viewCells.ts:111-114 constructor ViewCells.constructor
constructor(readonly spec: ViewCellsSpec<V>);

// src/world/viewCells.ts:117-119 method ViewCells.cellAt
/** The key of the cell holding a point. */
  cellAt(x: number, y: number): CellKey;

// src/world/viewCells.ts:148-150 method ViewCells.has
/** Whether a cell's product is held (made by this generator). */
  has(key: CellKey): boolean;

// src/world/viewCells.ts:161-170 method ViewCells.ensure
/** A cell's value, derived now if it is not held: what a question about a place far from the view costs (one cell), never the map. */
  ensure(key: CellKey): V;

// src/world/viewCells.ts:242-244 getter ViewCells.wantedKeys
/** The cells the view wants (`look`), in their order. */
  get wantedKeys(): readonly CellKey[];

// src/world/viewCells.ts:271-276 method ViewCells.clear
/** Every held cell dropped (a map loaded, a diary that cannot say where). */
  clear(): void;

// src/world/viewCells.ts:279-289 method ViewCells.follow
/** The document's changes since the last look, read off its diary: the cells they reached dropped (everything, when it cannot say where). The cells dropped. */
  follow(journal: ChangeJournal): number;

// src/world/viewCells.ts:292-314 method ViewCells.trim
/** Keeps the products to the budgets: the least recently used out first, never a cell the view wants nor one of `keep`. `cpuBudget`: a lower one to rest at (after a build swept the map). */
  trim(keep: ReadonlySet<CellKey> | null, cpuBudget = this.spec.cpuBudget): void;

// src/world/viewCells.ts:328-335 method ViewCells.drop
private drop(key: CellKey): void;

// src/world/viewCells.ts:338-341 method ViewCells.touch
/** The cell made the most recently used (last in the map's order, MDN: a key deleted and set again is inserted anew). */
  private touch(key: CellKey, known: Held<V>): void;
