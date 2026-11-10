// The code of src/world/roads/derivedSigns.ts that is the feature's (the rest of the file is shared).

// src/world/roads/derivedSigns.ts:27-61 slice of src/world/roads/derivedSigns.ts#signsAtNode (shared): the feature's statements; "…" marks code left out
export function signsAtNode(net: Network, nodeId: NodeId): DerivedSign[] {
  // … 5 lines
  for (const segId of [...node.incident].sort((a, b) => a - b)) {
    // … 10 lines
    const kind = approachSign(doc, node, segId);
    if (kind && arrivesAt(doc, node.id, segId)) {
      const at = Math.min(net.stopLineDistance(segId, node.id) + m(1), outward.length * 0.45);
      const f = outward.sampleAt(at);
      // Travelling toward the node (-t), the driver's right is +n of the outward frame.
      out.push({ type: kind, text: '', segment: segId, node: node.id,
        x: f.p.x + f.n.x * kerb, y: f.p.y + f.n.y * kerb, facing: { x: f.t.x, y: f.t.y } });
    }
    // … 1 lines
    if (!regional || node.incident.length === 2 || !arrivesAt(doc, segment.a === node.id ? segment.b : segment.a, segId)) continue;
  // … 5 lines
  }
// … 1 lines
}

// src/world/roads/derivedSigns.ts:67-83 slice of src/world/roads/derivedSigns.ts#signApproach (shared): the feature's statements; "…" marks code left out
export function signApproach(net: Network, at: Vec2, segment: SegmentId): { node: NodeId; segment: SegmentId } | null {
  // … 7 lines
  for (const [node, d] of [[seg.a, s], [seg.b, line.length - s]] as const) {
    // … 1 lines
    if (!n || n.incident.length < 3 || !arrivesAt(doc, node, segment)) continue;
  // … 3 lines
  }
// … 1 lines
}
