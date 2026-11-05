// What the feature uses of src/world/trees.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/trees.ts:10-18 interface PlantedTree
export interface PlantedTree {
  readonly x: number;
  readonly y: number;
  /** Its height, world units. */
  readonly height: number;
  readonly yaw: number;
  /** 0..1: which model (`render/natureTrees.ts` picks by it), its tint and lean. */
  readonly seed: number;
}
