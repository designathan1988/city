// The code of src/editor/repair.ts that is the feature's (the rest of the file is shared).

// src/editor/repair.ts:39-71 slice of src/editor/repair.ts#nearestCandidate (shared): the feature's statements; "…" marks code left out
function nearestCandidate(doc: RoadDoc, net: Network, rejected: ReadonlySet<string>): Candidate | null {
  // … 1 lines
  for (const node of doc.nodes.values()) {
    // … 7 lines
    for (const ribbon of ribbonsNear(net, [node.x, node.y, node.x, node.y])) {
      // … 9 lines
      const targetOffset = (doc.node(target.a)?.heightOffset ?? 0) * (1 - t) +
        (doc.node(target.b)?.heightOffset ?? 0) * t +
        roadStructure(target.structure).clearance;
      const sourceOffset = node.heightOffset + roadStructure(source.structure).clearance;
      if (Math.abs(sourceOffset - targetOffset) > m(0.3)) continue;
    // … 4 lines
    }
  }
// … 1 lines
}
