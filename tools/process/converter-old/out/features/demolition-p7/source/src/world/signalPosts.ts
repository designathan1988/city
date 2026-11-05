// The code of src/world/signalPosts.ts that is the feature's (the rest of the file is shared).

// src/world/signalPosts.ts:49-51 interface SignalJunctions: the feature's own
export interface SignalJunctions {
  readonly junctions: ReadonlyMap<NodeId, { readonly signalised: boolean; readonly groups: readonly { readonly segments: readonly SegmentId[] }[] }>;
}

// src/world/signalPosts.ts:53-76 function signalPosts: the feature's own
export function signalPosts(net: Network, graph: SignalJunctions, near?: { readonly x: number; readonly y: number; readonly radius: number }): SignalPost[] {
  const out: SignalPost[] = [];
  let nodes: NodeId[];
  if (near) {
    const ends = new Set<NodeId>();
    for (const ribbon of net.ribbonsMeeting([near.x - near.radius, near.y - near.radius, near.x + near.radius, near.y + near.radius])) {
      const s = net.doc.segment(ribbon.id);
      if (s) { ends.add(s.a); ends.add(s.b); }
    }
    nodes = [...ends].filter((node) => graph.junctions.has(node)).sort((a, b) => a - b);
  } else nodes = [...graph.junctions.keys()].sort((a, b) => a - b);
  for (const node of nodes) {
    const junction = graph.junctions.get(node);
    if (!junction?.signalised) continue;
    const record = net.doc.node(node);
    if (!record) continue;
    for (const segmentId of record.incident) {
      if (!junction.groups.some((group) => group.segments.includes(segmentId))) continue;
      const post = signalPostPlace(net, node, segmentId);
      if (post) out.push(post);
    }
  }
  return out;
}
