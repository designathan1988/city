// The code of src/editor/streetObjects.ts that is the feature's (the rest of the file is shared).

// src/editor/streetObjects.ts:72-105 slice of src/editor/streetObjects.ts#commitUturn (shared): the feature's statements; "…" marks code left out
export function commitUturn(doc: RoadDoc, net: Network, segmentId: SegmentId, toward: 'a' | 'b', s?: number): UturnResult {
  // … 27 lines
  const fits = LaneletGraph.junctionConnectors(work, workNet, node).connectors.some((c) => c.turn === 'uturn' && c.maxBodyClass >= car);
  if (!fits) return { committed: false, reason: 'narrow' };
// … 3 lines
}
