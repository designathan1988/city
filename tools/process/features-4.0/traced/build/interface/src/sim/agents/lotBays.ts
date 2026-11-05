// What the feature uses of src/sim/agents/lotBays.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lotBays.ts:15-15 variable STALL
/** DO NOT read the world here but through `BayReads`: the worker has none. */

/** Width of one stall, as the lot planner lays them (2.5 x 5 m). */
const STALL = m(2.5);

// src/sim/agents/lotBays.ts:17-17 variable LORRY_DOCK
/** A stall wider than this is a lorry's dock, not a car's bay. */
const LORRY_DOCK = m(3.2);

// src/sim/agents/lotBays.ts:19-19 variable LANE_REACH
/** How far from a point the lane beside it is looked for (`laneBeside`), u. */
export const LANE_REACH = m(45);

// src/sim/agents/lotBays.ts:23-23 variable CAR_GATE
/** The narrowest gate a car is driven through (a person's is 1.2 m); `parkingLayout.ts` `CAR_GATE_WIDTH`. */
const CAR_GATE = m(2.2);

// src/sim/agents/lotBays.ts:26-26 type LotBlock
/** What a lot's bays read of a block of its building (`bayRows`, `stallRoom`, `gatedProperty`). */
type LotBlock = Pick<Volume, 'x' | 'y' | 'w' | 'd' | 'open' | 'base'>;

// src/sim/agents/lotBays.ts:28-35 interface LotRecord
/** What a lot's bays read of its building's record: its frame, its blocks' boxes and its parts. */
export interface LotRecord {
  readonly id: BuildingId;
  readonly x: number;
  readonly y: number;
  readonly rotation: number;
  readonly volumes: readonly LotBlock[];
  readonly elements?: readonly LotPart[];
}

// src/sim/agents/lotBays.ts:38-38 type BayLaneLine
/** What a lot's bays read of a lane: a `Lanelet`'s own fields, on the page or made again in a worker. */
export type BayLaneLine = Pick<Lanelet, 'id' | 'kind' | 'length' | 'segment' | 'centre'>;

// src/sim/agents/lotBays.ts:41-45 interface WallRing
/** A building's solid footprint, world frame, with its bounds. */
export interface WallRing {
  readonly building: BuildingId;
  readonly ring: readonly { x: number; y: number }[];
  readonly x0: number; readonly y0: number; readonly x1: number; readonly y1: number;
}

// src/sim/agents/lotBays.ts:48-57 interface BayReads
/** Everything a lot's bays read of the world. */
export interface BayReads {
  /** The lanes whose bounds come within `reach` of a point, in id order (`LaneGrid.near`). */
  lanesNear(x: number, y: number, reach: number): readonly BayLaneLine[];
  /** Whether a lane is a ghost now (`SimWorld.rt`). */
  ghost(lane: LaneletId): boolean;
  /** Whether a street runs both ways (`RoadSegment.direction`). */
  bothWays(segment: SegmentId): boolean;
  /** The walls whose bounds meet a box. */
  wallsIn(x0: number, y0: number, x1: number, y1: number): readonly WallRing[];
}

// src/sim/agents/lotBays.ts:67-70 variable STALL_BLOCKS
/** Every part of a lot that a parked body may not stand in: what a car cannot drive through */
const STALL_BLOCKS: Readonly<Partial<Record<LotPart['kind'], true>>> = Object.freeze({
  wall: true, fence: true, hedge: true, railing: true, gate: true, pillar: true, stair: true, ramp: true, tree: true, bench: true,
  planter: true, rocks: true, clock: true, shrub: true, bin: true, lamp: true, bollard: true,
});

// src/sim/agents/lotBays.ts:72-72 variable OVER_A_BODY
/** Parts this high off the ground or more stand over a parked body (a canopy, a sign). */
const OVER_A_BODY = m(2.1);

// src/sim/agents/lotBays.ts:77-108 function stallRoom
/** Half the clear length of a stall (local frame of its building): from its centre `(lx, ly)` along `(dx, dy)` */
function stallRoom(b: LotRecord, lx: number, ly: number, dx: number, dy: number, half: number): number;

// src/sim/agents/lotBays.ts:127-127 variable EDGE_MARGIN
/** Clear of the corners of a lot where a car leaves it for the street. */
const EDGE_MARGIN = m(3.5);

// src/sim/agents/lotBays.ts:129-129 variable EDGE_INSET
/** How far inside the street edge the last turn is made. */
const EDGE_INSET = m(3);

// src/sim/agents/lotBays.ts:131-131 variable EDGE_REACH
/** Farthest a lot's street edge may be from the lane it opens onto. */
const EDGE_REACH = m(22);

// src/sim/agents/lotBays.ts:134-206 function bayRows
/** A lot's stalls, each with the way out of its lot and the lane it is reached from (ids from 0). */
export function bayRows(r: BayReads, b: LotRecord): Bay[];

// src/sim/agents/lotBays.ts:209-215 function wallsNear
/** The walls whose bounds come within a few metres of a lot. */
function wallsNear(r: BayReads, b: LotRecord, v: { x: number; y: number; w: number; d: number }): readonly WallRing[];

// src/sim/agents/lotBays.ts:218-228 interface LotExit
/** A way out of a lot: a point inside its street edge, and the lane it joins, clear of every building. */
export interface LotExit {
  /** Local frame, a few metres inside the edge. */
  readonly inner: { readonly x: number; readonly y: number };
  /** Local frame, just inside the edge: the line from `inner` to it must be free of fences. */
  readonly edge: { readonly x: number; readonly y: number };
  readonly lane: BayLane;
  /** Distance from the edge to the lane's joining point. */
  readonly out: number;
  /** Local frame, just outside a gate: where a car is once through it. */
  readonly outer?: { readonly x: number; readonly y: number };
}

// src/sim/agents/lotBays.ts:231-231 variable JOIN_PAST_GATE
/** Where a car leaving through a gate joins its lane, past the gate; where one arriving stops, before it. */
const JOIN_PAST_GATE = m(6);

// src/sim/agents/lotBays.ts:237-239 function carGates
/** A building's car gates: the `gate` elements a car fits through. */
export function carGates<E extends LotPart = BuildingElement>(b: { readonly elements?: readonly E[] }): E[];

// src/sim/agents/lotBays.ts:242-245 function personGates
/** A building's people's gates: the `gate` elements narrower than a car's (`carGates`). */
export function personGates<E extends LotPart = BuildingElement>(b: { readonly elements?: readonly E[] }): E[];

// src/sim/agents/lotBays.ts:248-259 function gatedProperty
/** The property of a lot closed by a boundary with car gates: the box of all its open blocks */
export function gatedProperty(b: Pick<LotRecord, 'volumes' | 'elements'>): { x: number; y: number; w: number; d: number } | null;

// src/sim/agents/lotBays.ts:262-288 function gateExits
/** The ways out of a gated lot: one per car gate, its inner point a few metres inside, its outer point just outside */
export function gateExits(r: BayReads, b: LotRecord, property: { x: number; y: number; w: number; d: number }): LotExit[];

// src/sim/agents/lotBays.ts:291-312 function gateLane
/** The lane in front of a gate, on the kerb side */
function gateLane(r: BayReads, x: number, y: number, eitherSide = false): BayLane | null;

// src/sim/agents/lotBays.ts:315-315 variable EXIT_STEP
/** Spacing of the points tried along a lot's sides. */
const EXIT_STEP = m(2);

// src/sim/agents/lotBays.ts:317-317 variable SWEEP
/** Half the width of the swept path a car needs between the lot and its lane. */
const SWEEP = m(1.4);

// src/sim/agents/lotBays.ts:320-337 function clearOfWalls
/** Whether a car can be driven straight from `a` to `b` without its body touching a building. */
function clearOfWalls(r: Pick<BayReads, 'wallsIn'>, a: { x: number; y: number }, b: { x: number; y: number },
  sweep = SWEEP, except: BuildingId | null = null): boolean;

// src/sim/agents/lotBays.ts:340-366 function exitsOf
/** The ways out of a lot onto a street */
function exitsOf(r: BayReads, b: LotRecord, v: { x: number; y: number; w: number; d: number }): LotExit[];

// src/sim/agents/lotBays.ts:369-389 function laneFor
/** The nearest drivable lane with room to join, its point beside `(x, y)`, on the kerb side. */
export function laneFor(r: BayReads, x: number, y: number, among?: Iterable<BayLaneLine>, segment?: SegmentId): BayLane | null;

// src/sim/agents/lotBays.ts:394-400 interface LotAsk
/** One lot to work out away from the page, as the page reads it: the record, the lanes and walls within its reach. */
export interface LotAsk {
  readonly building: LotRecord;
  /** The lanes within the lot's reach (`LaneGrid.near`), in id order, each with its flags now. */
  readonly lanes: readonly { readonly lane: Lanelet; readonly ghost: boolean; readonly bothWays: boolean }[];
  /** The walls within the lot's reach. */
  readonly walls: readonly WallRing[];
}

// src/sim/agents/lotBays.ts:406-422 interface BayShare
/** Lots to work out, as plain data for a worker (`postMessage` copies it, MDN "structured clone") */
export interface BayShare {
  readonly lanes: readonly {
    readonly id: LaneletId; readonly kind: Lanelet['kind']; readonly length: number; readonly segment?: SegmentId;
    readonly ghost: boolean; readonly bothWays: boolean; readonly xy: Float64Array;
  }[];
  readonly walls: readonly { readonly building: BuildingId; readonly ring: Float64Array; readonly x0: number; readonly y0: number; readonly x1: number; readonly y1: number }[];
  readonly kinds: readonly LotPart['kind'][];
  readonly lots: readonly {
    readonly id: BuildingId; readonly x: number; readonly y: number; readonly rotation: number;
    readonly volumes: readonly LotBlock[];
    /** Null: the record has no parts at all (`elements` absent). */
    readonly parts: Float64Array | null;
    readonly partKinds: Uint8Array | null;
    readonly lanes: Int32Array;
    readonly walls: Int32Array;
  }[];
}

// src/sim/agents/lotBays.ts:425-470 function shareOf
/** Lots put as a share (`BayShare`), each lane and wall once. */
export function shareOf(asks: readonly LotAsk[]): BayShare;
