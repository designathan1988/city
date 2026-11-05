// What the feature uses of src/world/buildings/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/geometry.ts:30-30 type Frame
/** What a building's frame is read from: its origin and rotation (a worker is sent no more, `sim/agents/lotBays.ts`). */
type Frame = Pick<Building, 'x' | 'y' | 'rotation'>;

// src/world/buildings/geometry.ts:32-36 function localToWorld
export function localToWorld(b: Frame, lx: number, ly: number): Vec2;

// src/world/buildings/geometry.ts:90-94 function levelHeight
/** Height of level `L`, world units. */
export function levelHeight(b: Building, level: number): number;

// src/world/buildings/geometry.ts:97-101 function levelElevation
/** Height of the floor of level `L` above the building's ground floor. */
export function levelElevation(b: Building, level: number): number;

// src/world/buildings/geometry.ts:117-117 function volumeElevation
/** Height of the floor of a block's level `level` above the building's ground floor, its lift included. */
export const volumeElevation = (b: Building, v: Volume, level: number): number => …;

// src/world/buildings/geometry.ts:120-128 function liftAt
/** The lift of the block a point of the plan is in, on `level`: where a floor's furniture, partitions and cores stand */
export function liftAt(b: Building, level: number, x: number, y: number): number;
