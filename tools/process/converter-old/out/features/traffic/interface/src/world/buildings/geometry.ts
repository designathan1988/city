// What the feature uses of src/world/buildings/geometry.ts (shared with the program): declarations as the source writes them, bodies left out.

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

// src/world/buildings/geometry.ts:73-77 function localDirToWorld
/** A local direction turned into the world. */
export function localDirToWorld(b: Frame, dx: number, dy: number): Vec2;

// src/world/buildings/geometry.ts:171-173 function solidFootprints
/** World corner rings of the BUILT ground volumes: what a person cannot walk through. */
export function solidFootprints(b: Building, grow = 0): Vec2[][];

// src/world/buildings/geometry.ts:269-269 type Bounds
type Bounds = { readonly minX: number; readonly minY: number; readonly maxX: number; readonly maxY: number };

// src/world/buildings/geometry.ts:277-283 function storedBounds
/** `buildingBounds` of a record held by the store, worked out once per record and margin. */
export function storedBounds(b: Building, grow = 0): Bounds;

// src/world/buildings/geometry.ts:614-614 variable GROUND_BAY_STRIDE
/** Numbers per bay in a `GroundBays` table, and where each lies. */
export const GROUND_BAY_STRIDE = 8;

// src/world/buildings/geometry.ts:615-615 variable GB_X,GB_Y,GB_NX,GB_NY,GB_Z,GB_SIDE,GB_WIDTH
export const GB_X = 0, GB_Y = 1, GB_NX = 2, GB_NY = 3, GB_Z = 4, GB_SIDE = 5, GB_WIDTH = 6;

// src/world/buildings/geometry.ts:619-619 type GroundBays
/** The ground storey's bays (level 0) of a building, packed */
export type GroundBays = Float64Array;

// src/world/buildings/geometry.ts:623-644 function groundBays
export function groundBays(b: Building): GroundBays;

// src/world/buildings/geometry.ts:647-648 function groundBayComponent
/** The component of a packed ground bay. */
export const groundBayComponent = (table: GroundBays, i: number): BayComponent => …;
