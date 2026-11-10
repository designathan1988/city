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
