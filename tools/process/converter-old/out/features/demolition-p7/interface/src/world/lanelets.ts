// What the feature uses of src/world/lanelets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/lanelets.ts:27-27 type LaneletId
export type LaneletId = string;

// src/world/lanelets.ts:28-28 type ConnectorId
export type ConnectorId = string;

// src/world/lanelets.ts:31-60 interface Lanelet
/** The atomic unit of vehicle space: a directed, single-lane centreline. */
export interface Lanelet {
  readonly id: LaneletId;
  readonly kind: 'link' | 'connector';
  readonly centre: Polyline;
  readonly length: number;
  /** Free-flow speed in world units per second. */
  readonly speedLimit: number;

  // link fields
  readonly segment?: SegmentId;
  readonly from?: NodeId;
  readonly to?: NodeId;
  readonly laneIndex?: number;
  /** The kerb-side lane of a street with a cycle lane (`cycle` band) on its right: how far sideways, in `perp` units */
  readonly cycleShift?: number;
  /** True when the far end of this link is a junction with a stop line. */
  readonly controlled: boolean;
  /** Buses only (`RoadSection.useForward`); absent: every vehicle. */
  readonly use?: 'bus';
  /** No lane change across the line to lane `laneIndex - 1` / `laneIndex + 1` (solid, or a bus lane's). */
  readonly solidInner?: true;
  readonly solidOuter?: true;

  // connector fields
  readonly node?: NodeId;
  readonly fromLane?: LaneletId;
  readonly toLane?: LaneletId;
  readonly turn?: TurnKind;
  readonly group?: number;
}

// src/world/lanelets.ts:62-76 interface Connector
export interface Connector {
  readonly id: ConnectorId;
  readonly lanelet: LaneletId;
  readonly node: NodeId;
  readonly fromLane: LaneletId;
  readonly toLane: LaneletId;
  readonly inSegment: SegmentId;
  readonly outSegment: SegmentId;
  readonly turn: TurnKind;
  /** True when this movement is the road the node is ON carried across it */
  readonly carried: boolean;
  readonly group: number;
  readonly length: number;
  readonly maxBodyClass: BodyClass | -1;
}

// src/world/lanelets.ts:78-87 interface JunctionTopology
export interface JunctionTopology {
  readonly node: NodeId;
  readonly groups: readonly ApproachGroup[];
  readonly connectors: readonly ConnectorId[];
  /** Inbound link lanelets, in deterministic order. */
  readonly inbound: readonly LaneletId[];
  readonly signalised: boolean;
  /** Explicit policy copied from the authoring node when available. */
  readonly control?: JunctionControl;
}

// src/world/lanelets.ts:131-135 interface GraphChange
/** Builds every lanelet and connector of the network. */
/** What one `LaneletGraph.update` built again: the junctions, the lanelets that came or went, the segments. */
export interface GraphChange {
  readonly junctions: ReadonlySet<NodeId>;
  readonly lanelets: ReadonlySet<LaneletId>;
  readonly segments: ReadonlySet<SegmentId>;
}

// src/world/lanelets.ts:137-815 class LaneletGraph
export class LaneletGraph { … }

// src/world/lanelets.ts:140-140 field LaneletGraph.junctions
readonly junctions = new Map<NodeId, JunctionTopology>();
