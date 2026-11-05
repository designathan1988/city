// What the feature uses of src/world/approachGroups.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/approachGroups.ts:5-11 interface ApproachGroup
/** A set of approach legs that can safely receive green together. */
export interface ApproachGroup {
  readonly id: number;
  /** Segments whose approach belongs to this group. */
  readonly segments: readonly SegmentId[];
  /** Mean outgoing direction, used only to order groups deterministically. */
  readonly meanAngle: number;
}

// src/world/approachGroups.ts:14-14 variable GROUP_MERGE_DEG
/** Angular tolerance for merging two legs into one approach group. */
const GROUP_MERGE_DEG = 35;
