// What the feature uses of src/world/changes.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/changes.ts:4-4 type ChangeRect
/** THE WORLD'S DIARY OF CHANGES: what changed, where, and why - one record for the whole game */

/** A world rectangle: [minX, minY, maxX, maxY]. */
export type ChangeRect = readonly [number, number, number, number];
