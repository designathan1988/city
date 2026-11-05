// What the feature uses of src/render/groundChanges.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/groundChanges.ts:4-4 type Rect
/** Where the drawn ground changed, and when */

/** A world rectangle: [minX, minY, maxX, maxY]. */
export type Rect = readonly [number, number, number, number];

// src/render/groundChanges.ts:77-79 function unionRect
/** Grows a rectangle to take another in (`null` is nothing yet). */
export function unionRect(a: Rect | null, b: Rect): Rect;
