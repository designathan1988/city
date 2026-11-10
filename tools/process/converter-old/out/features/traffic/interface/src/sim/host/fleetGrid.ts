// What the feature uses of src/sim/host/fleetGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/host/fleetGrid.ts:7-7 interface GroundBox
/** THE PAGE'S VEHICLES BY PLACE: a grid of 64 m cells (the world grid's sub-cell) the client files each view in as it decodes it */

/** A box on the ground, world x and y (three's -z), ends included. */
export interface GroundBox { x0: number; y0: number; x1: number; y1: number }

// src/sim/host/fleetGrid.ts:10-10 variable FLEET_CELL
/** The side of a cell. */
const FLEET_CELL = m(64);

// src/sim/host/fleetGrid.ts:14-14 function keyOf
const keyOf = (cx: number, cy: number): number => …;

// src/sim/host/fleetGrid.ts:17-17 interface FleetCell
/** A cell of the grid: its key, column and row, its items, and its version (moved, from one serial for the whole grid, each time an item comes, goes or changes in it: a cell let go and made again never shows an old version). */
export interface FleetCell<T> { readonly key: number; readonly cx: number; readonly cy: number; readonly items: T[]; version: number }

// src/sim/host/fleetGrid.ts:18-18 type Cell
type Cell<T> = FleetCell<T>;

// src/sim/host/fleetGrid.ts:22-121 class FleetGrid
/** Each item filed by where it stands now (`frames.x`, `frames.y`); refiled only when it changes cell. */
export class FleetGrid<T extends { … }

// src/sim/host/fleetGrid.ts:23-23 field FleetGrid.cells
private readonly cells = new Map<number, Cell<T>>();
