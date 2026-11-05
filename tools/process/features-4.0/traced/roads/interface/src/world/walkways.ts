// What the feature uses of src/world/walkways.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/walkways.ts:24-24 type WalkwayKind
/** WHERE PEOPLE WALK, as lanes: the pedestrian network of the city. */

type WalkwayKind = 'footway' | 'corner' | 'crossing';

// src/world/walkways.ts:32-54 interface Walkway
export interface Walkway {
  readonly id: number;
  readonly kind: WalkwayKind;
  /** Centreline, from node `a` to node `b`. People walk it either way. */
  readonly path: Polyline;
  /** Usable width, world units: the lateral room a walker keeps within (`hi - lo`). */
  readonly width: number;
  /** How far a walker's centreline position may lie either side of the path, u, measured along the path's LEFT normal */
  readonly lo: number;
  readonly hi: number;
  /** For a footway or corner, the side the kerb is on (-1 at `lo`, +1 at `hi`); 0 for a crossing. */
  readonly kerb: -1 | 0 | 1;
  readonly a: number;
  readonly b: number;
  /** The road a footway or crossing belongs to. */
  readonly segment?: SegmentId;
  /** The road node a corner or crossing belongs to. */
  readonly node?: NodeId;
  /** Which deck it is on. */
  readonly structure: RoadStructure;
  /** A crossing with no zebra painted: across the end of a road that leads nowhere. */
  readonly unmarked?: true;
}

// src/world/walkways.ts:60-66 function deckOf
/** The deck a road's footways belong to. */
export function deckOf(doc: RoadDoc, id: SegmentId): RoadStructure;

// src/world/walkways.ts:81-203 class WalkGraph
/** The walkways and their nodes, by id. */
export class WalkGraph { … }

// src/world/walkways.ts:223-239 interface FootwayShape
/** One side's footway of one road, mouth to mouth, as its road makes it (`footwayShapes`): read, never changed. */
interface FootwayShape {
  readonly segment: SegmentId;
  /** Side of the road: +1 left of the centreline as stored (a to b), -1 right. */
  readonly side: 1 | -1;
  readonly path: Polyline;
  readonly width: number;
  readonly lo: number;
  readonly hi: number;
  readonly kerb: -1 | 1;
  readonly structure: RoadStructure;
  /** How far the walking line lies beyond the kerb's outer edge, and the footway beyond it, u. */
  readonly inset: number;
  readonly outer: number;
  /** The path's frame at its two ends. */
  readonly first: { readonly p: Vec2; readonly t: Vec2 };
  readonly last: { readonly p: Vec2; readonly t: Vec2 };
}

// src/world/walkways.ts:247-262 interface FootwayEnd
interface FootwayEnd {
  readonly way: FootwayShape;
  /** Its node in the graph (none, -1, while an update is only planned: `Walkways.plan`). */
  readonly node: number;
  /** Side of the road: +1 left of the centreline as stored (a to b), -1 right. */
  readonly side: 1 | -1;
  /** How far the walking line lies beyond the kerb's outer edge, u. */
  readonly inset: number;
  /** How far the footway reaches beyond the walking line, away from the road, u. */
  readonly outer: number;
  /** Which side of a walker walking INTO the node the kerb is on: -1 its right, +1 its left. */
  readonly kerbSide: 1 | -1;
  /** The end at this road node, and the direction walking INTO the node. */
  readonly p: Vec2;
  readonly into: Vec2;
}

// src/world/walkways.ts:292-300 interface WalkPlan
/** What an update builds again (`Walkways.plan`), read before anything is changed. */
interface WalkPlan {
  readonly rawSegs: ReadonlySet<SegmentId>;
  readonly nodes: ReadonlySet<NodeId>;
  readonly cutSegs: ReadonlySet<SegmentId>;
  /** The footways of the roads built again that are there now. */
  readonly shapes: ReadonlyMap<SegmentId, readonly FootwayShape[]>;
  /** The corners of the nodes built again, for the workers (`Walkways.updateSteps`); none when the page rounds them. */
  readonly asks: readonly { readonly node: NodeId; readonly ask: CornerAsk }[];
}

// src/world/walkways.ts:312-315 method Walkways.update
/** The walkways of the network as it is now. */
  update(net: Network): WalkGraph;

// src/world/walkways.ts:318-328 method Walkways.updateSteps
/** DO NOT round them on the page: the kerbs' outlines merged and grown round every corner of a generated city held the */
  *updateSteps(net: Network, measurer?: CornerMeasurer): Generator<void | typeof WAITING, WalkGraph>;

// src/world/walkways.ts:331-371 method Walkways.plan
/** What an update builds again, read and not changed (`updateSteps` waits on the workers with it) */
  private plan(net: Network, ask: boolean): WalkPlan;

// src/world/walkways.ts:374-407 method Walkways.apply
/** An update built (`plan`); `rounded`: the corners of its nodes the workers rounded, in `cornersOf`'s order. */
  private apply(net: Network, plan: WalkPlan, rounded: ReadonlyMap<NodeId, readonly (Float64Array | null)[]> | null): WalkGraph;

// src/world/walkways.ts:446-559 method Walkways.buildNode
/** A road node's corners, crossings and road end; `rounded`: its corners' lines from the workers (`updateSteps`). */
  private buildNode(net: Network, nodeId: NodeId, rounded: readonly (Float64Array | null)[] | null): void;

// src/world/walkways.ts:562-562 function ascending
const ascending = (a: number, b: number): number => …;

// src/world/walkways.ts:565-588 function footwayShapes
/** A road's two footways, down the middle of each side's through zone, as shapes (`Walkways.footways` puts them in the graph). */
function footwayShapes(net: Network, segId: SegmentId): FootwayShape[];

// src/world/walkways.ts:591-604 function endsAt
/** The footway ends at a node, road by road in id order, each road's left side then its right; `nodeOf`: each end's node in the graph. */
function endsAt<T extends FootwayShape>(doc: RoadDoc, nodeId: NodeId, rawsOf: (seg: SegmentId) => readonly T[], nodeOf: (raw: T, end: 0 | 1) => number): FootwayEnd[];

// src/world/walkways.ts:607-612 function boxOf
/** The box a node's walking lines are read from: round the node and its footway ends. */
function boxOf(node: Vec2, here: readonly FootwayEnd[]): ChangeRect;

// src/world/walkways.ts:615-623 function ringsIn
/** The rings of the ribbons and junction plates filed in a box, a level and a deck at a time (`LocalLines`). */
function ringsIn(net: Network, rect: ChangeRect): RingsOf;

// src/world/walkways.ts:625-625 function endOf
const endOf = (e: FootwayEnd): CornerEnd => …;

// src/world/walkways.ts:627-627 function avoiding
/** The footway ends a corner keeps clear of: every other one at the node. */
const avoiding = (here: readonly FootwayEnd[], c: { readonly from: FootwayEnd; readonly to: FootwayEnd }): Vec2[] => …;

// src/world/walkways.ts:630-662 function cornersOf
/** The corners of a node, in the order they are joined: each from one footway end to another, with the kerb return's centre when there is one. */
function cornersOf(net: Network, nodeId: NodeId, here: readonly FootwayEnd[]): { readonly from: FootwayEnd; readonly to: FootwayEnd; readonly kerb: Vec2 | null }[];

// src/world/walkways.ts:665-673 function sideOf
/** The footway end of `seg` at this node on the given side of the leg (its normal `nrm`, sign +1 left). */
function sideOf(ends: readonly FootwayEnd[], seg: SegmentId, node: Vec2, nrm: Vec2, sign: 1 | -1): FootwayEnd | null;
