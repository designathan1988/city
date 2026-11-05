// What the feature uses of src/world/trees.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/trees.ts:4-4 variable TREE_KINDS
/** TREES THE PLAYER PLANTS, AND THE CLEARINGS THEY CUT. */

/** The tree models to choose from (`render/natureTrees.ts` TREE_MODELS, in order): each its share of the seed's range. */
export const TREE_KINDS = ['mixed', 'oak', 'cypress', 'palm'] as const;

// src/world/trees.ts:5-5 type TreeKind
export type TreeKind = (typeof TREE_KINDS)[number];

// src/world/trees.ts:27-36 interface TreeBrush
export interface TreeBrush {
  /** Trees per hectare at full strength. */
  readonly density: number;
  /** A tree's height, metres. */
  readonly height: number;
  /** 0..100: how far heights vary about it. */
  readonly variation: number;
  /** Least distance between two planted trees, metres. */
  readonly spacing: number;
}

// src/world/trees.ts:38-38 variable DEFAULT_TREE_BRUSH
export const DEFAULT_TREE_BRUSH: TreeBrush = { density: 120, height: 12, variation: 35, spacing: 6 };
