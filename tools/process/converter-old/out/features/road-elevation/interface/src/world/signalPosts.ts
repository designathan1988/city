// What the feature uses of src/world/signalPosts.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/signalPosts.ts:15-22 interface SignalPost
export interface SignalPost {
  readonly node: NodeId;
  readonly segment: SegmentId;
  readonly x: number;
  readonly y: number;
  /** Heading the arm points in, over the road (world angle). */
  readonly yaw: number;
}
