// The code of src/editor/roads/streetKit.ts that is the feature's (the rest of the file is shared).

// src/editor/roads/streetKit.ts:62-70 function removeWithStreetKit: the feature's own
export function removeWithStreetKit(doc: RoadDoc, net: Network, segments: readonly SegmentId[], edit: () => boolean): boolean {
  // The network brought up to the diary (`Network.rebuild` builds only what changed).
  net.rebuild();
  const kit = kitOf(doc, net, segments);
  if (!edit()) return false;
  for (const id of kit.poles) doc.removePole(id);
  for (const id of kit.items) doc.removeLandscape(id);
  return true;
}
