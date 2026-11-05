// What the feature uses of src/world/signalPosts.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/signalPosts.ts:11-11 variable SIGNAL_POST_KERB
/** Where the traffic signal posts stand */

/** Clearance from the carriageway edge to the post's centre line. */
export const SIGNAL_POST_KERB = m(0.9);

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

// src/world/signalPosts.ts:49-51 interface SignalJunctions
/** DO NOT ask for every post of the map to keep those round one point (a blast, `actionsWiring.ts`). */
/** What `signalPosts` reads of the junctions: the lanes' graph, or the page's view of its signals (`sim/host/simClient.ts`). */
export interface SignalJunctions {
  readonly junctions: ReadonlyMap<NodeId, { readonly signalised: boolean; readonly groups: readonly { readonly segments: readonly SegmentId[] }[] }>;
}

// src/world/signalPosts.ts:53-76 function signalPosts
export function signalPosts(net: Network, graph: SignalJunctions, near?: { readonly x: number; readonly y: number; readonly radius: number }): SignalPost[];
