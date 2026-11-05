// What the feature uses of src/world/walkways.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/walkways.ts:24-24 type WalkwayKind
/** WHERE PEOPLE WALK, as lanes: the pedestrian network of the city. */

type WalkwayKind = 'footway' | 'corner' | 'crossing';

// src/world/walkways.ts:26-30 interface WalkNode
export interface WalkNode {
  readonly id: number;
  readonly x: number;
  readonly y: number;
}

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

// src/world/walkways.ts:72-75 interface WalkChange
/** What one update of the walkways took away and put in (`WalkGraph.changesSince`). */
export interface WalkChange {
  readonly removed: readonly Walkway[];
  readonly added: readonly Walkway[];
}

// src/world/walkways.ts:81-203 class WalkGraph
/** The walkways and their nodes, by id. */
export class WalkGraph { … }

// src/world/walkways.ts:83-83 field WalkGraph.nodes
/** By id; a hole where an edit took one away (`holds`). */
  readonly nodes: (WalkNode | undefined)[] = [];

// src/world/walkways.ts:84-84 field WalkGraph.ways
readonly ways: (Walkway | undefined)[] = [];

// src/world/walkways.ts:86-86 field WalkGraph.at
/** The walkways meeting at each node. */
  readonly at = new Map<number, number[]>();

// src/world/walkways.ts:93-93 field WalkGraph.log
private readonly log: { readonly from: object; readonly change: WalkChange }[] = [];

// src/world/walkways.ts:96-96 field WalkGraph.token
private token: object = {};

// src/world/walkways.ts:100-100 getter WalkGraph.state
/** The graph's state now, for `changesSince`. */
  get state(): object;
