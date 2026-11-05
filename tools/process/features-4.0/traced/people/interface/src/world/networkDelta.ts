// What the feature uses of src/world/networkDelta.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/networkDelta.ts:5-9 interface NetworkDelta
/** What one rebuild of the network changed (`Network.rebuild`) */
export interface NetworkDelta {
  readonly segments: ReadonlySet<SegmentId>;
  readonly nodes: ReadonlySet<NodeId>;
  readonly rects: readonly ChangeRect[];
}
