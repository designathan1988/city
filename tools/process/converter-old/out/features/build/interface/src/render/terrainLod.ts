// What the feature uses of src/render/terrainLod.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/terrainLod.ts:28-32 interface LandGrid
/** A plate's grid as the levels read it: cells a side, their size and the plate's half side (`terrain.ts`). */
export interface LandGrid {
  readonly seg: number;
  readonly cell: number;
  readonly half: number;
}
