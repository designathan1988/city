// The code of src/world/roads/derivedSigns.ts that is the feature's (the rest of the file is shared).

// src/world/roads/derivedSigns.ts:67-83 function signApproach: the feature's own
export function signApproach(net: Network, at: Vec2, segment: SegmentId): { node: NodeId; segment: SegmentId } | null {
  const doc = net.doc;
  const seg = doc.segment(segment);
  if (!seg || seg.a === seg.b) return null;
  const line = net.polylines.get(doc, segment);
  const s = line.closestPoint(at).s;
  let best: { node: NodeId; segment: SegmentId } | null = null;
  let bestD = Infinity;
  for (const [node, d] of [[seg.a, s], [seg.b, line.length - s]] as const) {
    const n = doc.node(node);
    if (!n || n.incident.length < 3 || !arrivesAt(doc, node, segment)) continue;
    if (d > net.mouthDistance(segment, node) + APPROACH_REACH || d >= bestD) continue;
    best = { node, segment };
    bestD = d;
  }
  return best;
}
