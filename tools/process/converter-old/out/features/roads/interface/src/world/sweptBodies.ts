// What the feature uses of src/world/sweptBodies.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/sweptBodies.ts:5-5 interface Rect
/** Vehicle bodies as oriented rectangles, for the turn-path sweep (`turnPaths.ts`) */

export interface Rect { cx: number; cy: number; ux: number; uy: number; hl: number; hw: number; r: number }

// src/world/sweptBodies.ts:28-36 interface Queue
/** The car bodies standing in one waiting lane, and the box their centres span */
export interface Queue {
  readonly bodies: readonly Rect[];
  readonly minX: number;
  readonly minY: number;
  readonly maxX: number;
  readonly maxY: number;
  /** Bounding radius of every body in it (they are all one size). */
  readonly r: number;
}
