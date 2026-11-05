// The code of src/editor/streetObjects.ts that is the feature's (the rest of the file is shared).

// src/editor/streetObjects.ts:72-105 slice of src/editor/streetObjects.ts#commitUturn (shared): the feature's statements; "…" marks code left out
export function commitUturn(doc: RoadDoc, net: Network, segmentId: SegmentId, toward: 'a' | 'b', s?: number): UturnResult {
  // … 6 lines
  const reserve = MIN_LINK_LENGTH + MEDIAN_UTURN_OPENING;
  if (!Number.isFinite(at) || at < net.stopLineDistance(segmentId, segment.a) + reserve ||
      original.length - at < net.stopLineDistance(segmentId, segment.b) + reserve) {
    return { committed: false, reason: 'tooShort' };
  }
// … 21 lines
}
