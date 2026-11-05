// What the feature uses of src/world/buildings/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/geometry.ts:23-23 variable GRID
/** Every spatial question about a building, answered from the record. */

/** The step every horizontal dimension snaps to in the editor: half a metre. */
export const GRID = m(0.5);

// src/world/buildings/geometry.ts:30-30 type Frame
/** What a building's frame is read from: its origin and rotation (a worker is sent no more, `sim/agents/lotBays.ts`). */
type Frame = Pick<Building, 'x' | 'y' | 'rotation'>;

// src/world/buildings/geometry.ts:32-36 function localToWorld
export function localToWorld(b: Frame, lx: number, ly: number): Vec2;

// src/world/buildings/geometry.ts:39-44 interface BuildingFrame
/** DO NOT call `localToWorld` point by point in a building's loops */
export interface BuildingFrame {
  readonly x: number;
  readonly y: number;
  readonly c: number;
  readonly s: number;
}

// src/world/buildings/geometry.ts:59-65 function worldToLocal
export function worldToLocal(b: Frame, p: Vec2): Vec2;

// src/world/buildings/geometry.ts:97-101 function levelElevation
/** Height of the floor of level `L` above the building's ground floor. */
export function levelElevation(b: Building, level: number): number;

// src/world/buildings/geometry.ts:171-173 function solidFootprints
/** World corner rings of the BUILT ground volumes: what a person cannot walk through. */
export function solidFootprints(b: Building, grow = 0): Vec2[][];

// src/world/buildings/geometry.ts:757-772 function roofRise
/** How far a volume's roof rises above its top floor, world units. */
export function roofRise(b: Building, v: Volume): number;
