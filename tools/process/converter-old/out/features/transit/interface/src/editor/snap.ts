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

// src/editor/snap.ts:84-162 function findAnchor
/** @perInput Finds what the pointer is over: an existing node, a point on an existing road, or open ground. */
export function findAnchor(
  doc: RoadDoc,
  net: Network,
  p: Vec2,
  zoom: number,
  exclude?: ReadonlySet<NodeId>,
  targetHeightOffset?: number,
): Anchor;
