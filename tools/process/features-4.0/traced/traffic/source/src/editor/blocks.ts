// The code of src/editor/blocks.ts that is the feature's (the rest of the file is shared).

// src/editor/blocks.ts:32-49 slice of src/editor/blocks.ts#commitBlockGrid (shared): the feature's statements; "…" marks code left out
export function commitBlockGrid(
  doc: RoadDoc, net: Network, centre: Vec2, grid: BlockGrid, type: number, lanes: number | null, parking?: SegmentParking,
): number {
  // … 2 lines
  for (const [a, b] of blockGridLines(centre, grid)) {
    // … 6 lines
    const result = commitRoadPath(doc, net, start, end, type,
      [{ start: { at: start.at, heightOffset: 0 }, end: { at: end.at, heightOffset: 0 }, curve: null }], lanes, parking);
    if (result.committed) laid++;
  }
// … 1 lines
}
