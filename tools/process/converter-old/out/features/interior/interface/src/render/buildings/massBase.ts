// What the feature uses of src/render/buildings/massBase.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/massBase.ts:25-32 interface BasePiece
/**
 * THE BUILDINGS' TOP LEVEL (the base, above the plates): every building of a plate's square as the prisms of its real blocks - each
 * block's own footprint stood from its floor to its eaves (a pitched roof flat at half its rise), its paved lots flat - merged into one
 * mesh a square, in the leanest vertex a mass can have: four unsigned shorts, its place on the square's grid and its colour in the
 * fourth (`paint`, 5-6-5 bits of sRGB), shaded by each face's own slope (`flatShading`), each triangle in the colour of its last corner
 * (a `flat` varying: the walls and the roof share their top corners). Made first at the opening and never let go (`massCells.ts`):
 * drawn under every cell nothing finer draws (a level not resident, let go for the budget, not come yet), so no view shows a hole in
 * the town. Its triangles run cell by cell (`BasePiece.cells`): the cells a finer level draws are left out of its draw (three's
 * geometry groups), never drawn twice. Measured on the city of 25,000 buildings (2026-10-21): 6.6 MB, 277 B a building (53 MB for 200,000).
 */

/** A run of a base's cells whose vertices fit 16-bit indices (meshoptimizer's advice: half the index memory). */
export interface BasePiece {
  /** Four unsigned shorts a vertex: its place on the base's grid (`BaseMass.place`) read normalised, and its paint (`paint`). */
  readonly vertices: Uint16Array;
  readonly index: Uint16Array | Uint32Array;
  /** Four numbers a cell of the piece that has buildings: its column, its row, its first index and its count of indices (0: none of its buildings stands from afar). In index order. */
  readonly cells: Int32Array;
  readonly bounds: Bounds;
}

// src/render/buildings/massBase.ts:35-39 interface BaseMass
/** A square's base: its pieces on one grid. */
export interface BaseMass {
  readonly pieces: readonly BasePiece[];
  readonly place: ShellGrid;
  readonly top: number;
}

// src/render/buildings/massBase.ts:42-42 variable BASE_CELL_STRIDE
/** Numbers a cell's entry holds in `BasePiece.cells`. */
export const BASE_CELL_STRIDE = 4;

// src/render/buildings/massBase.ts:44-44 variable BASE_VERTEX
/** Shorts a base vertex holds. */
export const BASE_VERTEX = 4;
