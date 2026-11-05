// What the feature uses of src/render/buildings/lawnGround.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/lawnGround.ts:4-4 interface GroundLattice
/** The highest the drawn ground rises under a part laid on a lawn. */

/** The terrain grid the ground is drawn on: cell size and the half width (world units). */
export interface GroundLattice { readonly cell: number; readonly half: number }

// src/render/buildings/lawnGround.ts:6-62 function highestGroundUnder
export function highestGroundUnder(
  corners: readonly (readonly [number, number])[],
  groundAt: (x: number, y: number) => number,
  lattice: GroundLattice,
): number;
