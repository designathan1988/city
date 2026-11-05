// What the feature uses of src/world/buildings/streetDoors.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/streetDoors.ts:9-19 interface StreetDoor
/** The doors of a building that open straight onto the street: a shop or a bar built up to the footway */
export interface StreetDoor {
  /** The middle of the threshold, on the facade line. */
  readonly x: number;
  readonly y: number;
  /** Outward unit normal: the way out onto the street. */
  readonly nx: number;
  readonly ny: number;
  readonly width: number;
  /** The building's own floor at the door, above its ground floor (a block at a floor of its own). */
  readonly z: number;
}
