// What the feature uses of src/editor/snap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/snap.ts:14-14 type AnchorKind
type AnchorKind = 'node' | 'segment' | 'free';

// src/editor/snap.ts:16-23 interface Anchor
export interface Anchor {
  readonly kind: AnchorKind;
  readonly at: Vec2;
  readonly node?: NodeId;
  readonly segment?: SegmentId;
  /** Arc position along the segment, for `segment` anchors. */
  readonly s?: number;
}

// src/editor/snap.ts:25-31 interface SnapResult
export interface SnapResult {
  readonly at: Vec2;
  /** What produced the snap, for the on-screen guide label. */
  readonly guide: 'network' | 'continue' | 'perpendicular' | 'orthogonal' | 'angle' | null;
  readonly angleDeg: number;
  readonly length: number;
}
