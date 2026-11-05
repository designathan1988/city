// What the feature uses of src/world/roads/connectors.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/connectors.ts:5-12 interface LaneLink
/** LANE CONNECTORS BY HAND */
export interface LaneLink {
  /** The arriving road and its lane (laneIndex: 0 innermost). */
  readonly from: SegmentId;
  readonly fromLane: number;
  /** The leaving road and its lane. */
  readonly to: SegmentId;
  readonly toLane: number;
}

// src/world/roads/connectors.ts:36-36 function movementKey
/** Stable segment-pair key for a movement through a junction (`RoadNode.blockedMovements`). */
export const movementKey = (from: SegmentId, to: SegmentId): string => …;

// src/world/roads/connectors.ts:46-47 function linksOf
/** The links of one arriving lane. */
export const linksOf = (links: readonly LaneLink[] | undefined, from: SegmentId, fromLane: number): LaneLink[] => …;

// src/world/roads/connectors.ts:50-51 function linksDigest
/** What a node's connections are, for its key: '' when it has none of its own. */
export const linksDigest = (links: readonly LaneLink[] | undefined): string => …;

// src/world/roads/connectors.ts:54-59 interface NodeLanes
/** The arriving and leaving lanes a connector editor offers, and the connections now built (`lanelets.ts`). */
export interface NodeLanes {
  readonly node: NodeId;
  readonly arriving: readonly { readonly segment: SegmentId; readonly lane: number }[];
  readonly leaving: readonly { readonly segment: SegmentId; readonly lane: number }[];
  readonly built: readonly LaneLink[];
}

// src/world/roads/connectors.ts:80-83 interface LaneEnds
/** A junction's lanes as the connector editor draws them: `NodeLanes`, each lane's end at the junction and its next point, the graph's revision. */
export interface LaneEnds extends NodeLanes {
  readonly revision: number;
  readonly ends: readonly { readonly segment: SegmentId; readonly lane: number; readonly arriving: boolean; readonly at: Vec2; readonly next: Vec2 }[];
}
