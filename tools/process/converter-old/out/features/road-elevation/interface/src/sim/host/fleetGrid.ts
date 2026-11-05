// What the feature uses of src/sim/host/fleetGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/fleetGrid.ts:7-7 interface GroundBox
/** THE PAGE'S VEHICLES BY PLACE: a grid of 64 m cells (the world grid's sub-cell) the client files each view in as it decodes it */

/** A box on the ground, world x and y (three's -z), ends included. */
export interface GroundBox { x0: number; y0: number; x1: number; y1: number }

// src/sim/host/fleetGrid.ts:22-121 class FleetGrid
/** Each item filed by where it stands now (`frames.x`, `frames.y`); refiled only when it changes cell. */
export class FleetGrid<T extends { … }

// src/sim/host/fleetGrid.ts:85-101 method FleetGrid.near
/** The items whose current pose stands in the box, appended to `out`: the cells the box covers, or the filled cells when they are fewer. */
  near(x0: number, y0: number, x1: number, y1: number, out: T[]): T[];
