// What the feature uses of src/sim/agents/hybridAStar.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/hybridAStar.ts:8-17 interface Steered
/** A long vehicle's way through a tight yard */

/** A vehicle as the planner sees it. World units. */
export interface Steered {
  readonly length: number;
  readonly width: number;
  /** How far the body's centre is ahead of the rear axle. */
  readonly rear: number;
  /** The least radius the rear axle turns at (full lock). */
  readonly radius: number;
  /** Kept clear of anything solid, besides the body itself. */
  readonly margin: number;
}

// src/sim/agents/hybridAStar.ts:20-20 interface AxlePose
/** A pose of the rear axle: where it is and where the body points. */
export interface AxlePose { readonly x: number; readonly y: number; readonly angle: number }

// src/sim/agents/hybridAStar.ts:23-23 interface Waypoint
/** A pose on the way, and whether it is reached backing. */
export interface Waypoint extends AxlePose { readonly reverse: boolean }

// src/sim/agents/hybridAStar.ts:26-33 interface Solid
/** What is solid, on a grid: 1 for a cell a body may not touch. */
export interface Solid {
  readonly x0: number;
  readonly y0: number;
  readonly cell: number;
  readonly nx: number;
  readonly ny: number;
  readonly blocked: Uint8Array;
}

// src/sim/agents/hybridAStar.ts:36-38 interface Clearance
/** Distance from each cell's centre to the nearest solid cell's centre (0 in one), world units. */
export interface Clearance extends Solid {
  readonly clear: Float32Array;
}
