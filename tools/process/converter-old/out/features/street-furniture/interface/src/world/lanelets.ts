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

// src/world/lanelets.ts:108-117 interface JunctionCache
interface JunctionCache {
  readonly node: NodeId;
  /** The turns its movements were shaped with (`turnPath`): kept with the junction, gone with it. */
  readonly turns: JunctionTurns;
  readonly groups: readonly ApproachGroup[];
  readonly signalised: boolean;
  /** The connector lanelets and the connectors, in creation order. */
  readonly lanelets: readonly Lanelet[];
  readonly connectors: readonly Connector[];
}

// src/world/lanelets.ts:137-815 class LaneletGraph
export class LaneletGraph { … }

// src/world/lanelets.ts:138-138 field LaneletGraph.lanelets
readonly lanelets = new Map<LaneletId, Lanelet>();

// src/world/lanelets.ts:139-139 field LaneletGraph.connectors
readonly connectors = new Map<ConnectorId, Connector>();

// src/world/lanelets.ts:140-140 field LaneletGraph.junctions
readonly junctions = new Map<NodeId, JunctionTopology>();

// src/world/lanelets.ts:447-540 method LaneletGraph.buildSegmentLinks
/** The link lanelets of one segment, both directions (`buildLinks`), under the keys they were built from. */
  private buildSegmentLinks(doc: RoadDoc, net: Network, segId: SegmentId, previous: Map<string, Lanelet[]>, next: Map<string, Lanelet[]>): void;

// src/world/lanelets.ts:696-713 method LaneletGraph.buildJunction
/** The connectors of one node (`buildJunctions`): kept whole when its key is known, built otherwise (`junctionBuild.ts`). */
  private buildJunction(doc: RoadDoc, net: Network, nodeId: NodeId, node: RoadNode,
    previous: Map<string, JunctionCache>, next: Map<string, JunctionCache>, before: (node: NodeId) => { readonly turns: JunctionTurns } | undefined): void;
