// What the feature uses of src/world/buildings/foundation.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/foundation.ts:20-20 type GroundAt
export type GroundAt = (x: number, y: number) => number;

// src/world/buildings/foundation.ts:23-23 type PavedAt
/** Height of a PAVED surface at a point - a footway, a carriageway - or NaN where there is none. */
export type PavedAt = (x: number, y: number) => number;

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
