// The code of src/world/roads/streetNames.ts that is the feature's (the rest of the file is shared).

// src/world/roads/streetNames.ts:44-54 slice of src/world/roads/streetNames.ts#platesNear (shared): the feature's statements; "…" marks code left out
function platesNear(net: Network, chain: readonly SegmentId[]): LandscapeItem[] {
  const found = new Map<number, LandscapeItem>();
  for (const id of chain) {
    // … 2 lines
    for (const item of net.doc.landscapeIn([box.minX - NAME_REACH, box.minY - NAME_REACH, box.maxX + NAME_REACH, box.maxY + NAME_REACH])) {
      if (item.kind === 'streetname') found.set(item.id, item);
    }
  }
  return [...found.values()].sort((a, b) => a.id - b.id);
}

// src/world/roads/streetNames.ts:57-66 slice of src/world/roads/streetNames.ts#streetNameOf (shared): the feature's statements; "…" marks code left out
export function streetNameOf(net: Network, segment: SegmentId): string | null {
  // … 2 lines
  for (const item of platesNear(net, ids)) {
    if (!item.text) continue;
    // … 1 lines
    if (seg !== null && chain.has(seg)) return item.text;
  }
// … 1 lines
}
