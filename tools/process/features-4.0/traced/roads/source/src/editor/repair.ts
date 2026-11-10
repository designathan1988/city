// The code of src/editor/repair.ts that is the feature's (the rest of the file is shared).

// src/editor/repair.ts:19-37 slice of src/editor/repair.ts#repairNearConnections (shared): the feature's statements; "…" marks code left out
export function repairNearConnections(doc: RoadDoc, net: Network): number {
  // … 4 lines
  for (let attempt = 0; attempt < 64; attempt++) {
    net.rebuild();
    const candidate = nearestCandidate(doc, net, rejected);
    if (!candidate) break;
    const junction = splitSegment(doc, net, candidate.segment, candidate.s, candidate.at);
    if (junction === null || !doc.mergeNodes(junction, candidate.node)) {
      rejected.add(`${candidate.node}:${candidate.segment}`);
      continue;
    }
  // … 1 lines
  }
  if (repaired) net.rebuild();
// … 1 lines
}

// src/editor/repair.ts:39-71 slice of src/editor/repair.ts#nearestCandidate (shared): the feature's statements; "…" marks code left out
function nearestCandidate(doc: RoadDoc, net: Network, rejected: ReadonlySet<string>): Candidate | null {
  // … 1 lines
  for (const node of doc.nodes.values()) {
    // … 7 lines
    for (const ribbon of ribbonsNear(net, [node.x, node.y, node.x, node.y])) {
      const targetId = ribbon.id;
      const target = doc.segment(targetId);
      if (!target) continue;
      if (targetId === sourceId || target.a === node.id || target.b === node.id) continue;
      if (target.structure !== source.structure) continue;
      if (rejected.has(`${node.id}:${targetId}`)) continue;
      const line = net.polylines.get(doc, targetId);
      const hit = line.closestPoint({ x: node.x, y: node.y });
      const t = hit.s / Math.max(1e-6, line.length);
      const targetOffset = (doc.node(target.a)?.heightOffset ?? 0) * (1 - t) +
        (doc.node(target.b)?.heightOffset ?? 0) * t +
        roadStructure(target.structure).clearance;
      const sourceOffset = node.heightOffset + roadStructure(source.structure).clearance;
      if (Math.abs(sourceOffset - targetOffset) > m(0.3)) continue;
      // The ribbon's own profile, made from this segment (`makeRibbon`).
      const reach = casingHalf(ribbon.road);
      if (hit.distance > reach || (best && hit.distance >= best.distance)) continue;
      best = { node: node.id, segment: targetId, s: hit.s, at: hit.point, distance: hit.distance };
    }
  }
  return best;
}
