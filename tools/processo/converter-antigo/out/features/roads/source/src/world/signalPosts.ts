// The code of src/world/signalPosts.ts that is the feature's (the rest of the file is shared).

// src/world/signalPosts.ts:25-45 slice of src/world/signalPosts.ts#signalPostPlace (shared): the feature's statements; "…" marks code left out
export function signalPostPlace(net: Network, node: NodeId, segmentId: SegmentId): SignalPost | null {
  const segment = net.doc.segment(segmentId);
  if (!segment) return null;
  const road = roadProfile(segment.type, segment.lanes, segment.direction, segment.section, segment.parking);
  const mouth = net.mouthDistance(segmentId, node);
  const polyline = net.polylines.get(net.doc, segmentId);
  // … 1 lines
  const outward = segment.a === node ? polyline : polyline.reversed();
  // … 1 lines
  const record = net.doc.node(node);
  const midBlock = record?.crossing !== undefined && record.incident.length === 2;
  const distance = midBlock
    ? Math.min(net.stopLineDistance(segmentId, node), outward.length * 0.45)
    : Math.min(crosswalkDistance(mouth) + m(2), outward.length * 0.45);
  const frame = outward.sampleAt(distance);
  const travel = { x: -frame.t.x, y: -frame.t.y };
  // … 2 lines
  const position = addScaled(frame.p, right, road.width / 2 + SIGNAL_POST_KERB);
  return { node, segment: segmentId, x: position.x, y: position.y, yaw: angleOf({ x: -right.x, y: -right.y }) };
}

// src/world/signalPosts.ts:53-76 slice of src/world/signalPosts.ts#signalPosts (shared): the feature's statements; "…" marks code left out
export function signalPosts(net: Network, graph: SignalJunctions, near?: { readonly x: number; readonly y: number; readonly radius: number }): SignalPost[] {
  // … 2 lines
  if (near) {
    // … 1 lines
    for (const ribbon of net.ribbonsMeeting([near.x - near.radius, near.y - near.radius, near.x + near.radius, near.y + near.radius])) {
      const s = net.doc.segment(ribbon.id);
      if (s) { ends.add(s.a); ends.add(s.b); }
    }
  // … 1 lines
  } else nodes = [...graph.junctions.keys()].sort((a, b) => a - b);
  for (const node of nodes) {
    // … 2 lines
    const record = net.doc.node(node);
    if (!record) continue;
    for (const segmentId of record.incident) {
      if (!junction.groups.some((group) => group.segments.includes(segmentId))) continue;
      const post = signalPostPlace(net, node, segmentId);
      if (post) out.push(post);
    }
  }
// … 1 lines
}
