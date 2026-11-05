// What the feature uses of src/sim/peds/sidewalk.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/peds/sidewalk.ts:16-16 type SidewalkNodeId
export type SidewalkNodeId = string;

// src/sim/peds/sidewalk.ts:17-17 type SidewalkEdgeId
export type SidewalkEdgeId = string;

// src/sim/peds/sidewalk.ts:28-28 type SidewalkEdgeKind
type SidewalkEdgeKind = 'walk' | 'crossing';

// src/sim/peds/sidewalk.ts:30-44 interface SidewalkEdge
export interface SidewalkEdge {
  readonly id: SidewalkEdgeId;
  readonly kind: SidewalkEdgeKind;
  readonly from: SidewalkNodeId;
  readonly to: SidewalkNodeId;
  readonly path: Polyline;
  readonly length: number;
  /** Crossings only: the junction being crossed at. */
  readonly node?: NodeId;
  /** The leg: the road a crossing goes over, or the road a footway line runs beside. */
  readonly segment?: SegmentId;
  readonly crossing?: CrossingId;
  /** Crossings only: the vehicle lanelets this edge passes over. */
  readonly lanes?: readonly LaneletId[];
}

// src/sim/peds/sidewalk.ts:47-263 class SidewalkGraph
/** The kerbs, zebras and footway lines the vehicles and signals read */
export class SidewalkGraph { … }

// src/sim/peds/sidewalk.ts:49-49 field SidewalkGraph.edges
readonly edges = new Map<SidewalkEdgeId, SidewalkEdge>();

// src/sim/peds/sidewalk.ts:50-50 field SidewalkGraph.crossings
readonly crossings = new Map<CrossingId, SidewalkEdgeId>();

// src/sim/peds/sidewalk.ts:52-52 field SidewalkGraph.walksBySegment
/** The footway lines (`kind: 'walk'`) of each road, in the order they were added to `edges`. */
  private readonly walksBySegment = new Map<SegmentId, SidewalkEdge[]>();

// src/sim/peds/sidewalk.ts:55-55 field SidewalkGraph.ofNode
/** The kerb nodes and zebras each road node made: what an edit there takes away. */
  private readonly ofNode = new Map<NodeId, { readonly kerbs: SidewalkNodeId[]; readonly crossings: CrossingId[] }>();

// src/sim/peds/sidewalk.ts:58-63 method SidewalkGraph.build
/** @rebuild Every road and node: a world's first topology, or a lane graph built whole. */
  build(doc: RoadDoc, net: Network, graph: LaneletGraph): void;

// src/sim/peds/sidewalk.ts:66-98 method SidewalkGraph.update
/** DO NOT build the kerbs, zebras and footway lines of the whole map for an edit: every road drawn made the town's again. */
  update(doc: RoadDoc, net: Network, graph: LaneletGraph, changes: readonly GraphChange[] | null): void;

// src/sim/peds/sidewalk.ts:101-139 method SidewalkGraph.kerbsAt
/** The kerb nodes of one road node: two per leg. */
  private kerbsAt(doc: RoadDoc, net: Network, nodeId: NodeId): void;

// src/sim/peds/sidewalk.ts:142-174 method SidewalkGraph.crossingsAt
/** The zebras of one road node: one per leg, over its own road's link lanes. */
  private crossingsAt(doc: RoadDoc, net: Network, graph: LaneletGraph, nodeId: NodeId): void;
