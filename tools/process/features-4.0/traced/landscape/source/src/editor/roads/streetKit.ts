// The code of src/editor/roads/streetKit.ts that is the feature's (the rest of the file is shared).

// src/editor/roads/streetKit.ts:14-14 slice of src/editor/roads/streetKit.ts#carried (shared): the feature's statements; "…" marks code left out
const carried = (kind: LandscapeKind): boolean => FURNITURE_CATALOG.some((e) => e.kind === kind);

// src/editor/roads/streetKit.ts:41-59 slice of src/editor/roads/streetKit.ts#kitOf (shared): the feature's statements; "…" marks code left out
function kitOf(doc: RoadDoc, net: Network, segments: readonly SegmentId[]): Kit {
  // … 8 lines
  for (const id of segments) {
    // … 5 lines
    for (const item of doc.landscapeIn(rect)) if (carried(item.kind) && !items.includes(item.id) && owned(item)) items.push(item.id);
  }
// … 1 lines
}
