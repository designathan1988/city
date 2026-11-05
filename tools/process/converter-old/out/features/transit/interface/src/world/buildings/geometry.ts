// What the feature uses of src/world/buildings/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/geometry.ts:97-101 function levelElevation
/** Height of the floor of level `L` above the building's ground floor. */
export function levelElevation(b: Building, level: number): number;

// src/world/buildings/geometry.ts:153-162 function volumeCorners
/** The four world corners of a volume, anticlockwise in the local frame. */
export function volumeCorners(b: Building, v: Volume, grow = 0): Vec2[];

// src/world/buildings/geometry.ts:171-173 function solidFootprints
/** World corner rings of the BUILT ground volumes: what a person cannot walk through. */
export function solidFootprints(b: Building, grow = 0): Vec2[][];

// src/world/buildings/geometry.ts:757-772 function roofRise
/** How far a volume's roof rises above its top floor, world units. */
export function roofRise(b: Building, v: Volume): number;
