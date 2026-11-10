// What the feature uses of src/sim/agents/laneGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/laneGrid.ts:8-8 variable LANE_CELL
/** Cell of the lanes' grid, world units. */
const LANE_CELL = m(30);

// src/sim/agents/laneGrid.ts:10-10 function cellKey
const cellKey = (gx: number, gy: number): number => …;

// src/sim/agents/laneGrid.ts:13-17 function cellsMeet
/** Whether the grid finds a lane of these bounds for a box (`LaneGrid.near`, `lanesIn`) */
export function cellsMeet(box: { readonly minX: number; readonly minY: number; readonly maxX: number; readonly maxY: number },
  x0: number, y0: number, x1: number, y1: number): boolean;

// src/sim/agents/laneGrid.ts:18-18 function byId
const byId = (a: Lanelet, b: Lanelet): number => …;

// src/sim/agents/laneGrid.ts:20-27 interface Filed
interface Filed {
  readonly lane: Lanelet;
  readonly cells: readonly number[];
  /** The lane's digest (`digest`), worked out when first asked; NaN until then. */
  digest: number;
  /** The query that last met it (`LaneGrid.visit`): a lane in several of a query's cells is taken once. */
  visit: number;
}

// src/sim/agents/laneGrid.ts:30-150 class LaneGrid
/** DO NOT file every lane of the map again when the graph moves */
export class LaneGrid { … }

// src/sim/agents/laneGrid.ts:31-31 field LaneGrid.filed
private readonly filed = new Map<LaneletId, Filed>();

// src/sim/agents/laneGrid.ts:32-32 field LaneGrid.cells
private readonly cells = new Map<number, LaneletId[]>();

// src/sim/agents/laneGrid.ts:42-58 method LaneGrid.follow
/** Brought up to the lane graph's changes since it was last. */
  follow(graph: LaneletGraph, log: ChangeLog<GraphChange>): void;

// src/sim/agents/laneGrid.ts:61-66 method LaneGrid.fileAll
/** @rebuild Every lane filed again. */
  private fileAll(graph: LaneletGraph): void;

// src/sim/agents/laneGrid.ts:68-81 method LaneGrid.file
private file(lane: Lanelet): void;

// src/sim/agents/laneGrid.ts:83-93 method LaneGrid.unfile
private unfile(was: Filed): void;

// src/sim/agents/laneGrid.ts:96-100 method LaneGrid.lanesIn
/** The lanes filed in the cells a box covers, in id order. */
  private lanesIn(x0: number, y0: number, x1: number, y1: number): Lanelet[];

// src/sim/agents/laneGrid.ts:103-117 method LaneGrid.gather
/** The lanes filed in the cells a box covers, each once, into `out` in no set order: DO NOT make a set and a list of them for each query (SIM-28). */
  private gather(x0: number, y0: number, x1: number, y1: number, out: Lanelet[]): void;

// src/sim/agents/laneGrid.ts:120-124 method LaneGrid.nearInto
/** `near`'s lanes into `out`, in NO set order: for a reader whose answer does not depend on it (`SimWorld.vehiclesNear` sorts its vehicles itself). */
  nearInto(x: number, y: number, reach: number, out: Lanelet[]): void;

// src/sim/agents/laneGrid.ts:127-130 method LaneGrid.near
/** The lanes whose bounds come within `reach` of a point (their cells), in id order (the tie-break) */
  near(x: number, y: number, reach: number): readonly Lanelet[];

// src/sim/agents/laneGrid.ts:133-141 method LaneGrid.inBox
/** The lanes whose bounds meet a box, in id order: what a loop over every lane with the same bounds test keeps. */
  inBox(x0: number, y0: number, x1: number, y1: number): Lanelet[];

// src/sim/agents/laneGrid.ts:144-149 method LaneGrid.digest
/** A lane's part of a lot's key (`parking.ts` lotBaysOf) */
  digest(lane: Lanelet): number;

// src/sim/agents/laneGrid.ts:152-152 function digestOf
const digestOf = (lane: Lanelet): number => …;

// src/sim/agents/laneGrid.ts:154-157 function boxOf
function boxOf(lane: Lanelet): ChangeRect;

// src/sim/agents/laneGrid.ts:160-164 interface LaneGridWorld
/** DO NOT import `SimWorld`: `sim/world.ts` imports this file, and the type import closed an import cycle */
export interface LaneGridWorld {
  readonly graph: LaneletGraph;
  readonly topologyLog: ChangeLog<GraphChange>;
  readonly laneGrid: LaneGrid;
}

// src/sim/agents/laneGrid.ts:167-170 function laneGrid
/** The world's lanes' grid (`SimWorld.laneGrid`), brought up to the lane graph's changes. */
export function laneGrid(w: LaneGridWorld): LaneGrid;

// src/sim/agents/laneGrid.ts:173-175 function lanesNear
/** The lanes whose bounds come within `reach` of a point, in id order (`LaneGrid.near`). */
export function lanesNear(w: LaneGridWorld, x: number, y: number, reach: number): readonly Lanelet[];

// src/sim/agents/laneGrid.ts:178-180 function lanesNearInto
/** `lanesNear` into `out`, in no set order (`LaneGrid.nearInto`). */
export function lanesNearInto(w: LaneGridWorld, x: number, y: number, reach: number, out: Lanelet[]): void;

// src/sim/agents/laneGrid.ts:183-185 function lanesInBox
/** The lanes whose bounds meet a box, in id order (`LaneGrid.inBox`). */
export function lanesInBox(w: LaneGridWorld, x0: number, y0: number, x1: number, y1: number): Lanelet[];
