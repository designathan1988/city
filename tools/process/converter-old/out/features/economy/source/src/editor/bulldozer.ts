// The code of src/editor/bulldozer.ts that is the feature's (the rest of the file is shared).

// src/editor/bulldozer.ts:28-36 slice of src/editor/bulldozer.ts#removeRoads (shared): the feature's statements; "…" marks code left out
export function removeRoads(doc: RoadDoc, net: Network, ids: readonly SegmentId[]): boolean {
  return removeWithStreetKit(doc, net, ids, () => {
    const money = roadsBefore(doc);
    // … 2 lines
    settleRoadEdit(money, doc);
  // … 1 lines
  });
}
