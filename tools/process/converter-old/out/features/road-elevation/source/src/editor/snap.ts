// The code of src/editor/snap.ts that is the feature's (the rest of the file is shared).

// src/editor/snap.ts:84-162 slice of src/editor/snap.ts#findAnchor (shared): the feature's statements; "…" marks code left out
export function findAnchor(
// … 6 lines
): Anchor {
  // … 34 lines
  for (const ribbon of near) {
    // … 19 lines
    if (targetHeightOffset !== undefined) {
      // … 3 lines
      const height = a + (b - a) * fraction + roadStructure(seg.structure).clearance;
      if (Math.abs(height - targetHeightOffset) > m(0.3)) continue;
    }
  // … 6 lines
  }
// … 2 lines
}

// src/editor/snap.ts:403-417 slice of src/editor/snap.ts#anchorHeightOffset (shared): the feature's statements; "…" marks code left out
export function anchorHeightOffset(doc: RoadDoc, net: Network, anchor: Anchor, fallback: number): number {
  // … 3 lines
  if (anchor.kind === 'segment' && anchor.segment !== undefined) {
    // … 6 lines
    return a + (b - a) * t + roadStructure(segment.structure).clearance;
  }
// … 1 lines
}
