// The code of src/world/roads/laneEnds.ts that is the feature's (the rest of the file is shared).

// src/world/roads/laneEnds.ts:8-24 slice of src/world/roads/laneEnds.ts#nodeLanes (shared): the feature's statements; "…" marks code left out
export function nodeLanes(graph: LaneletGraph, node: NodeId): NodeLanes | null {
  const junction = graph.junctions.get(node);
  if (!junction) return null;
  // … 4 lines
  const arriving = (graph.inbound.get(node) ?? []).map(lane).filter((x): x is { segment: SegmentId; lane: number } => x !== null);
  const leaving = (graph.outbound.get(node) ?? []).map(lane).filter((x): x is { segment: SegmentId; lane: number } => x !== null);
  // … 1 lines
  for (const id of junction.connectors) {
    const c = graph.connectors.get(id);
    const a = c && lane(c.fromLane), b = c && lane(c.toLane);
    if (a && b) built.push({ from: a.segment, fromLane: a.lane, to: b.segment, toLane: b.lane });
  }
  return { node, arriving, leaving, built };
}

// src/world/roads/laneEnds.ts:11-14 slice of src/world/roads/laneEnds.ts#nodeLanes.lane (shared): the feature's statements; "…" marks code left out
  const lane = (id: string): { segment: SegmentId; lane: number } | null => {
    const l = graph.lanelet(id);
    return l && l.segment !== undefined ? { segment: l.segment, lane: l.laneIndex ?? 0 } : null;
  };

// src/world/roads/laneEnds.ts:27-41 slice of src/world/roads/laneEnds.ts#laneEnds (shared): the feature's statements; "…" marks code left out
export function laneEnds(graph: LaneletGraph, node: NodeId): LaneEnds | null {
  const lanes = nodeLanes(graph, node);
  if (!lanes) return null;
  // … 1 lines
  for (const [ids, arriving] of [[graph.inbound.get(node) ?? [], true], [graph.outbound.get(node) ?? [], false]] as const) {
    for (const id of ids) {
      const l = graph.lanelet(id);
      if (!l || l.segment === undefined) continue;
      const pts = l.centre.toPoints();
      const at = arriving ? pts[pts.length - 1]! : pts[0]!, next = arriving ? pts[Math.max(0, pts.length - 2)]! : pts[Math.min(pts.length - 1, 1)]!;
      ends.push({ segment: l.segment, lane: l.laneIndex ?? 0, arriving, at: { x: at.x, y: at.y }, next: { x: next.x, y: next.y } });
    }
  }
  return { ...lanes, revision: graph.revision, ends };
}
