// What the feature uses of src/world/buildings/foundation.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/foundation.ts:20-20 type GroundAt
export type GroundAt = (x: number, y: number) => number;

// src/world/buildings/foundation.ts:23-23 type PavedAt
/** Height of a PAVED surface at a point - a footway, a carriageway - or NaN where there is none. */
export type PavedAt = (x: number, y: number) => number;

// src/world/buildings/foundation.ts:32-32 variable ENTRANCE_REACH
/** How far in front of an entrance paving still counts as the street it opens onto. */
const ENTRANCE_REACH = m(1.5);

// src/world/buildings/foundation.ts:34-34 variable FLOOR_READ_REACH
/** The farthest `floorHeight` reads the ground or the paving past a building's bounds */
export const FLOOR_READ_REACH = ENTRANCE_REACH + m(0.1);
