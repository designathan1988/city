// What the feature uses of src/world/buildings/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/geometry.ts:30-30 type Frame
/** What a building's frame is read from: its origin and rotation (a worker is sent no more, `sim/agents/lotBays.ts`). */
type Frame = Pick<Building, 'x' | 'y' | 'rotation'>;

// src/world/buildings/geometry.ts:59-65 function worldToLocal
export function worldToLocal(b: Frame, p: Vec2): Vec2;

// src/world/buildings/geometry.ts:97-101 function levelElevation
/** Height of the floor of level `L` above the building's ground floor. */
export function levelElevation(b: Building, level: number): number;

// src/world/buildings/geometry.ts:117-117 function volumeElevation
/** Height of the floor of a block's level `level` above the building's ground floor, its lift included. */
export const volumeElevation = (b: Building, v: Volume, level: number): number => …;

// src/world/buildings/geometry.ts:171-173 function solidFootprints
/** World corner rings of the BUILT ground volumes: what a person cannot walk through. */
export function solidFootprints(b: Building, grow = 0): Vec2[][];

// src/world/buildings/geometry.ts:757-772 function roofRise
/** How far a volume's roof rises above its top floor, world units. */
export function roofRise(b: Building, v: Volume): number;
