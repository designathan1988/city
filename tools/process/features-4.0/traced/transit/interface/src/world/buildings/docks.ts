// What the feature uses of src/world/buildings/docks.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/docks.ts:7-16 interface LorryDock
/** A works' lorry docks: the wide bays its loading yard is laid with, each square in front of a loading door of the hall */
export interface LorryDock {
  /** The dock's element (its stall on the yard). */
  readonly element: number;
  /** The middle of its end at the loading door, and of its open end. */
  readonly door: Vec2;
  readonly open: Vec2;
  readonly width: number;
  /** Its height over the building's ground floor (a yard at a level of its own). */
  readonly z: number;
}
