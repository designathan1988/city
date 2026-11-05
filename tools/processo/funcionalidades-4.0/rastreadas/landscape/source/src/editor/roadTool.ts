// The code of src/editor/roadTool.ts that is the feature's (the rest of the file is shared).

// src/editor/roadTool.ts:65-100 slice of src/editor/roadTool.ts#RoadToolHost (shared): the feature's statements; "…" marks code left out
export interface RoadToolHost {
  // … 23 lines
  offerElevation(solution: RoadElevation, revision: number): void;
  // … 1 lines
  groundSolve?(): RoadElevation | null;
// … 8 lines
}

// src/editor/roadTool.ts:572-581 slice of src/editor/roadTool.ts#landscapeAlong (shared): the feature's statements; "…" marks code left out
function landscapeAlong(doc: RoadDoc, net: Network, laid: readonly SegmentId[]): LandscapeItem[] {
  const out = new Map<number, LandscapeItem>();
  for (const id of laid) {
    // … 3 lines
    for (const item of doc.landscapeIn([box.minX - pad, box.minY - pad, box.maxX + pad, box.maxY + pad])) out.set(item.id, item);
  }
  return [...out.values()].sort((a, b) => a.id - b.id);
}
