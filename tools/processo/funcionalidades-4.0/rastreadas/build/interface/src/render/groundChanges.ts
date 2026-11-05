// What the feature uses of src/render/groundChanges.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/groundChanges.ts:4-4 type Rect
/** Where the drawn ground changed, and when */

/** A world rectangle: [minX, minY, maxX, maxY]. */
export type Rect = readonly [number, number, number, number];

// src/render/groundChanges.ts:10-15 interface GroundRecord
/** What a dependant reads: the serial of the latest change, and whether a change since one reaches an area. */
export interface GroundRecord {
  readonly version: number;
  touches(since: number, area: Area): boolean;
  /** Where the ground changed after `since`; null when the record cannot say (the whole map, or forgotten). */
  rectsSince?(since: number): readonly Rect[] | null;
}

// src/render/groundChanges.ts:18-18 field GroundChanges.serial
private serial = 0;

// src/render/groundChanges.ts:20-20 field GroundChanges.log
/** Changes after `forgotten`, oldest first; `null` is the whole map. */
  private readonly log: { readonly serial: number; readonly rect: Rect | null }[] = [];

// src/render/groundChanges.ts:21-21 field GroundChanges.forgotten
private forgotten = 0;

// src/render/groundChanges.ts:56-66 method GroundChanges.rectsSince
rectsSince(since: number): readonly Rect[] | null;

// src/render/groundChanges.ts:70-70 type Area
/** What a dependant stands on: one rectangle, its items' rectangles, or nothing. */
export type Area = Rect | readonly Rect[] | null;

// src/render/groundChanges.ts:77-79 function unionRect
/** Grows a rectangle to take another in (`null` is nothing yet). */
export function unionRect(a: Rect | null, b: Rect): Rect;
