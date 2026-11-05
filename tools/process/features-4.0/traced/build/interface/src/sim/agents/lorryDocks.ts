// What the feature uses of src/sim/agents/lorryDocks.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lorryDocks.ts:32-34 function lorryOf
/** The lorry as the planner sees it. */
export function lorryOf(a: Pick<Archetype, 'length' | 'width'>): Steered;

// src/sim/agents/lorryDocks.ts:42-42 variable MOUTH_FLARE
/** The paved mouth in front of a gate a lorry may cross the footway by: the gate's width and this much more either side. */
const MOUTH_FLARE = m(2);

// src/sim/agents/lorryDocks.ts:46-46 variable STOP_BEFORE
/** The lorry stops this far before the gate on its lane and joins the lane this far past it (a lorry swings wider than a car). */
const STOP_BEFORE = m(9);

// src/sim/agents/lorryDocks.ts:47-47 variable JOIN_PAST
const JOIN_PAST = m(12);

// src/sim/agents/lorryDocks.ts:49-49 variable PRE_DOCK
/** Farthest from the dock its last straight reverse starts (nearer leaves the lorry more of the yard to turn its nose in: 40 docks of 77 against 25 at 3 m, in a test block). */
const PRE_DOCK = m(1);

// src/sim/agents/lorryDocks.ts:54-54 variable PLAN_BUDGET
/** Poses expanded a tick, across all the yards being planned: under a millisecond (a dock's ways, some 50 000 poses, in a few seconds of play). */
export const PLAN_BUDGET = 150;

// src/sim/agents/lorryDocks.ts:57-60 variable SOLID
/** What a lorry cannot drive through: what a car cannot (`lotNav.ts`), less the docks themselves. */
const SOLID: ReadonlySet<ElementKind> = new Set<ElementKind>([
  'stair', 'ramp', 'pillar', 'wall', 'fence', 'tree', 'bench', 'planter', 'railing', 'rocks', 'clock', 'hedge',
  'shrub', 'bin', 'lamp', 'bollard', 'parking',
]);

// src/sim/agents/lorryDocks.ts:62-62 variable OVER_A_LORRY
/** Parts this high or higher stand over a lorry (3.5 m tall). */
const OVER_A_LORRY = m(4.2);

// src/sim/agents/lorryDocks.ts:64-64 type Plan
type Plan = { search: WaySearch | null; way: Waypoint[] | null; done: boolean };

// src/sim/agents/lorryDocks.ts:66-74 interface YardGate
export interface YardGate {
  readonly exit: LotExit;
  readonly lane: BayLane;
  /** Where the lorry stands on its lane before the gate, and joins it past the gate (local, rear axle). */
  readonly stop: AxlePose;
  readonly stopAt: number;
  readonly join: AxlePose;
  readonly joinAt: number;
}

// src/sim/agents/lorryDocks.ts:76-87 interface YardDock
export interface YardDock {
  readonly dock: LorryDock;
  /** The lorry on the dock (local, rear axle), and the start of its last straight reverse. */
  readonly docked: AxlePose;
  readonly pre: AxlePose;
  /** Through which gate, and the ways: from the stop before the gate to `pre`; from `pre` out to the gate's join. */
  gate: number;
  readonly onto: Plan[];
  readonly away: Plan[];
  /** The gates in the order they are tried: the nearest the dock first. */
  readonly order: number[];
}

// src/sim/agents/lorryDocks.ts:90-94 interface LorryWay
/** A way a lorry drives, in the world: rear-axle poses with the direction each is reached in. */
export interface LorryWay {
  readonly poses: readonly Waypoint[];
  /** Where, along `poses`, the street part ends (arriving) or begins (leaving): the lanes it crosses are closed meanwhile. */
  readonly street: number;
}

// src/sim/agents/lorryDocks.ts:96-272 class Yard
export class Yard { … }

// src/sim/agents/lorryDocks.ts:249-252 method Yard.toWorld
private toWorld(p: Waypoint): Waypoint;

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

// src/sim/agents/lorryDocks.ts:292-294 function lotsNear
/** DO NOT file the town's lots again for a yard: the lot store's own index by place, kept by its writers (`RoadDoc.lotsIn`). */
function lotsNear(w: SimWorld, x0: number, y0: number, x1: number, y1: number): Lot[];

// src/sim/agents/lorryDocks.ts:297-305 function ownLot
/** A building's own lot among those near it: the last in the lots' order, as one filing of the whole map kept it. */
function ownLot(w: SimWorld, b: TrafficBuilding, lots: readonly Lot[]): Lot | undefined;

// src/sim/agents/lorryDocks.ts:308-376 function buildYard
/** Makes a yard a slice at a time: what is solid to a lorry filled in its grid - from what is near the yard only */
export function* buildYard(b: TrafficBuilding, key: string, w: SimWorld, body: Steered): Generator<void, Yard, void>;

// src/sim/agents/lorryDocks.ts:379-387 function yardBox
/** A yard's grid (local frame: its corner, cells across and down, its box) and the grid's box in the world. */
function yardBox(b: TrafficBuilding, prop: { x: number; y: number; w: number; d: number });

// src/sim/agents/lorryDocks.ts:393-410 function yardInputs
/** What a yard is made from, near it, as one number: the lanes its gates may be joined to */
export function yardInputs(w: SimWorld, b: TrafficBuilding, lots?: readonly Lot[]): number;

// src/sim/agents/lorryDocks.ts:425-439 function knownYard
/** DO NOT throw a yard away on `yardKey` alone, nor on a lot changed anywhere: its inputs are read again only when the roads moved or the diary places a lot or a building change on its grid */
export function knownYard(w: SimWorld, b: TrafficBuilding): Yard | null | undefined;

// src/sim/agents/lorryDocks.ts:442-444 function forgetYard
/** A building's yard forgotten: its building gone, or no longer one with a yard (`lorryTraffic.ts` YardList). */
export function forgetYard(w: SimWorld, building: number): void;

// src/sim/agents/lorryDocks.ts:447-451 function forgetYardsBut
/** @rebuild The yards of the buildings `keep` turns down forgotten: when the town's yards were read whole. */
export function forgetYardsBut(w: SimWorld, keep: (building: number) => boolean): void;

// src/sim/agents/lorryDocks.ts:472-472 type AsphaltPiece
/** The carriageways' polygons, each with its box in the world: a yard takes in only those near it. */
type AsphaltPiece = { readonly rings: MultiPoly[number]; readonly box: { x0: number; y0: number; x1: number; y1: number } };

// src/sim/agents/lorryDocks.ts:475-489 function asphaltNear
/** DO NOT merge the whole map's carriageways for a yard (`levelPolygons`) */
function asphaltNear(net: Network, x0: number, y0: number, x1: number, y1: number): AsphaltPiece[];

// src/sim/agents/lorryDocks.ts:492-494 function gateDistance
/** How far a gate's inside is from a dock's open end. */
function gateDistance(gate: YardGate, open: { x: number; y: number }): number;

// src/sim/agents/lorryDocks.ts:496-503 function nearestGate
function nearestGate(gates: readonly TrafficPart[], x: number, y: number): TrafficPart | undefined;

// src/sim/agents/lorryDocks.ts:505-510 function elementRing
function elementRing(el: LotPart): { x: number; y: number }[];

// src/sim/agents/lorryDocks.ts:512-515 function localDir
function localDir(b: TrafficBuilding, dx: number, dy: number): { x: number; y: number };
