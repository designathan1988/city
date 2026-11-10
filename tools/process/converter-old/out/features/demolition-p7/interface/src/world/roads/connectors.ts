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
