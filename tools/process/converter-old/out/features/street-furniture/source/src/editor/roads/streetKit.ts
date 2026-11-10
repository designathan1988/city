// The code of src/editor/roads/streetKit.ts that is the feature's (the rest of the file is shared).

// src/editor/roads/streetKit.ts:14-14 slice of src/editor/roads/streetKit.ts#carried (shared): the feature's statements; "…" marks code left out
const carried = (kind: LandscapeKind): boolean => FURNITURE_CATALOG.some((e) => e.kind === kind);

// src/editor/roads/streetKit.ts:41-59 slice of src/editor/roads/streetKit.ts#kitOf (shared): the feature's statements; "…" marks code left out
function kitOf(doc: RoadDoc, net: Network, segments: readonly SegmentId[]): Kit {
  // … 8 lines
  for (const id of segments) {
    // … 4 lines
    for (const pole of doc.polesIn(rect)) if (!poles.includes(pole.id) && owned(pole)) poles.push(pole.id);
  // … 1 lines
  }
// … 1 lines
}

// src/editor/roads/streetKit.ts:73-90 slice of src/editor/roads/streetKit.ts#carryStreetKit (shared): the feature's statements; "…" marks code left out
export function carryStreetKit(doc: RoadDoc, net: Network, segments: readonly SegmentId[], edit: () => boolean,
  after: () => readonly SegmentId[] = () => segments.filter((id) => doc.segment(id) !== undefined)): boolean {
  // … 12 lines
  const set: FurnitureSet = kit.line ? 'complete' : 'basic';
  if (kit.items.length) for (const piece of furnitureFor(net, streets, set, null, power)) doc.addLandscape(piece.kind, piece.at);
// … 1 lines
}
