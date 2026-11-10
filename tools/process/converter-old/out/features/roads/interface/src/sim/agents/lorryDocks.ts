// What the feature uses of src/sim/agents/lorryDocks.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lorryDocks.ts:46-46 variable STOP_BEFORE
/** The lorry stops this far before the gate on its lane and joins the lane this far past it (a lorry swings wider than a car). */
const STOP_BEFORE = m(9);

// src/sim/agents/lorryDocks.ts:47-47 variable JOIN_PAST
const JOIN_PAST = m(12);

// src/sim/agents/lorryDocks.ts:90-94 interface LorryWay
/** A way a lorry drives, in the world: rear-axle poses with the direction each is reached in. */
export interface LorryWay {
  readonly poses: readonly Waypoint[];
  /** Where, along `poses`, the street part ends (arriving) or begins (leaving): the lanes it crosses are closed meanwhile. */
  readonly street: number;
}

// src/sim/agents/lorryDocks.ts:275-283 interface YardGrid
/** A yard's grid as `buildYard` fills it, before its gates and docks. */
interface YardGrid {
  readonly inputs: number;
  readonly reach: ChangeRect;
  readonly seen: number;
  readonly ring: { x: number; y: number }[];
  readonly exits: LotExit[];
  readonly solid: Solid;
  readonly field: Clearance;
}

// src/sim/agents/lorryDocks.ts:390-390 variable GATE_LANES_READ
/** How far from a yard's grid the lanes its gates may be joined to are read (`parking.ts` gateLane's reach, and a margin). */
const GATE_LANES_READ = m(45) + m(5);

// src/sim/agents/lorryDocks.ts:393-410 function yardInputs
/** What a yard is made from, near it, as one number: the lanes its gates may be joined to */
export function yardInputs(w: SimWorld, b: TrafficBuilding, lots?: readonly Lot[]): number;

// src/sim/agents/lorryDocks.ts:422-422 function yardKey
/** The roads as they stand: a yard known good for them needs no look (`knownYard`). */
export const yardKey = (w: SimWorld): string => …;

// src/sim/agents/lorryDocks.ts:472-472 type AsphaltPiece
/** The carriageways' polygons, each with its box in the world: a yard takes in only those near it. */
type AsphaltPiece = { readonly rings: MultiPoly[number]; readonly box: { x0: number; y0: number; x1: number; y1: number } };

// src/sim/agents/lorryDocks.ts:475-489 function asphaltNear
/** DO NOT merge the whole map's carriageways for a yard (`levelPolygons`) */
function asphaltNear(net: Network, x0: number, y0: number, x1: number, y1: number): AsphaltPiece[];

// src/sim/agents/lorryDocks.ts:512-515 function localDir
function localDir(b: TrafficBuilding, dx: number, dy: number): { x: number; y: number };
