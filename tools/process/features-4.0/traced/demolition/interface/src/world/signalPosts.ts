// What the feature uses of src/world/signalPosts.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/signalPosts.ts:13-13 variable SIGNAL_POST_RADIUS
/** Radius of the post: the drawn pole, 0.165 m (the pedestrians do not read it yet, see above). */
export const SIGNAL_POST_RADIUS = m(0.17);

// src/world/signalPosts.ts:15-22 interface SignalPost
export interface SignalPost {
  readonly node: NodeId;
  readonly segment: SegmentId;
  readonly x: number;
  readonly y: number;
  /** Heading the arm points in, over the road (world angle). */
  readonly yaw: number;
}

// src/world/signalPosts.ts:25-45 function signalPostPlace
/** The post of one approach, or null if the segment is gone. */
export function signalPostPlace(net: Network, node: NodeId, segmentId: SegmentId): SignalPost | null;
