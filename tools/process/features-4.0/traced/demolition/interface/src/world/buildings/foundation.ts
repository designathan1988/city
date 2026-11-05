// What the feature uses of src/world/buildings/foundation.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/foundation.ts:20-20 type GroundAt
export type GroundAt = (x: number, y: number) => number;

// src/world/buildings/foundation.ts:23-23 type PavedAt
/** Height of a PAVED surface at a point - a footway, a carriageway - or NaN where there is none. */
export type PavedAt = (x: number, y: number) => number;

// src/world/buildings/foundation.ts:289-293 function floorHeight
/** The absolute ground-floor height of a building, uncached. Same rule as `foundationOf`. */
export function floorHeight(b: Building, groundAt: GroundAt, pavedAt: PavedAt = NO_PAVING): number;
