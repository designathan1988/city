// What the feature uses of src/world/buildings/pick.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/pick.ts:8-15 interface Ray3
/** A ray, in WORLD axes: x and y on the map, z up. */
export interface Ray3 {
  readonly ox: number;
  readonly oy: number;
  readonly oz: number;
  readonly dx: number;
  readonly dy: number;
  readonly dz: number;
}
