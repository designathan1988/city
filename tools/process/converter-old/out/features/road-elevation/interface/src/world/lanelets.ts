// What the feature uses of src/world/lanelets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/lanelets.ts:27-27 type LaneletId
export type LaneletId = string;

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

// src/world/lanelets.ts:447-540 method LaneletGraph.buildSegmentLinks
/** The link lanelets of one segment, both directions (`buildLinks`), under the keys they were built from. */
  private buildSegmentLinks(doc: RoadDoc, net: Network, segId: SegmentId, previous: Map<string, Lanelet[]>, next: Map<string, Lanelet[]>): void;
