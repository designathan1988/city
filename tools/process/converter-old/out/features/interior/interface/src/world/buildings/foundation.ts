// What the feature uses of src/world/buildings/foundation.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/foundation.ts:12-12 variable PLINTH_BURY
/** The plinth runs this far below the lowest ground, so no slope shows under it. */
export const PLINTH_BURY = m(0.8);

// src/world/buildings/foundation.ts:16-16 variable LANDING_RISE
/** DO NOT lay the landing at the floor itself. */
export const LANDING_RISE = m(0.012);

// src/world/buildings/foundation.ts:17-17 variable STEP_RISE
export const STEP_RISE = m(0.17);

// src/world/buildings/foundation.ts:18-18 variable STEP_RUN
export const STEP_RUN = m(0.3);

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

// src/world/buildings/foundation.ts:41-64 interface Entrance
/** Extension point: a way into the building on its ground floor. */
export interface Entrance {
  readonly volume: number;
  readonly side: FaceId;
  readonly index: number;
  readonly component: BayComponent;
  /** World point of the threshold, on the facade line. */
  readonly x: number;
  readonly y: number;
  readonly nx: number;
  readonly ny: number;
  readonly width: number;
  /** How far its bay stands out from (negative: is set back into) its side, by a relief. */
  readonly push: number;
  /** Absolute height of the ground at the foot of the steps. */
  readonly ground: number;
  /** Absolute height of its landing, the top of its flight: `LANDING_RISE` over the floor it opens from */
  readonly floor: number;
  /** Number of steps from the ground up to the floor; 0 = level access. */
  readonly steps: number;
  /** How far behind the facade line the threshold is, when the flight is set into the building */
  readonly recess: number;
  /** Length of the slab at `ground` height from the foot of a recessed flight out to the paving, over the verge between them */
  readonly threshold: number;
}

// src/world/buildings/foundation.ts:66-76 interface Foundation
export interface Foundation {
  /** Absolute world height of the ground floor. */
  readonly floor: number;
  readonly lowest: number;
  readonly highest: number;
  /** Absolute height of the bottom of the plinth. */
  readonly bottom: number;
  /** Ground spread under the footprint: what `MAX_PLINTH` limits. */
  readonly spread: number;
  readonly entrances: readonly Entrance[];
}

// src/world/buildings/foundation.ts:289-293 function floorHeight
/** The absolute ground-floor height of a building, uncached. Same rule as `foundationOf`. */
export function floorHeight(b: Building, groundAt: GroundAt, pavedAt: PavedAt = NO_PAVING): number;
