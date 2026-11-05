// What the feature uses of src/world/buildings/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/geometry.ts:39-44 interface BuildingFrame
/** DO NOT call `localToWorld` point by point in a building's loops */
export interface BuildingFrame {
  readonly x: number;
  readonly y: number;
  readonly c: number;
  readonly s: number;
}

// src/world/buildings/geometry.ts:171-173 function solidFootprints
/** World corner rings of the BUILT ground volumes: what a person cannot walk through. */
export function solidFootprints(b: Building, grow = 0): Vec2[][];
