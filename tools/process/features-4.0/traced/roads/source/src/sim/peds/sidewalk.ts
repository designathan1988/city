// The code of src/sim/peds/sidewalk.ts that is the feature's (the rest of the file is shared).

// src/sim/peds/sidewalk.ts:30-44 slice of src/sim/peds/sidewalk.ts#SidewalkEdge (shared): the feature's statements; "…" marks code left out
export interface SidewalkEdge {
  // … 12 lines
  readonly lanes?: readonly LaneletId[];
}

// src/sim/peds/sidewalk.ts:58-63 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.build (shared): the feature's statements; "…" marks code left out
  build(doc: RoadDoc, net: Network, graph: LaneletGraph): void {
    // … 1 lines
    for (const nodeId of doc.nodes.keys()) this.kerbsAt(doc, net, nodeId);
    for (const nodeId of doc.nodes.keys()) this.crossingsAt(doc, net, graph, nodeId);
    for (const segId of doc.segments.keys()) this.footwaysOf(doc, net, segId);
  }

// src/sim/peds/sidewalk.ts:66-98 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.update (shared): the feature's statements; "…" marks code left out
  update(doc: RoadDoc, net: Network, graph: LaneletGraph, changes: readonly GraphChange[] | null): void {
    if (changes === null) { this.build(doc, net, graph); return; }
    // … 1 lines
    for (const change of changes) {
      for (const node of change.junctions) nodes.add(node);
      for (const seg of change.segments) roads.add(seg);
    }
    // … 22 lines
    for (const nodeId of order) this.kerbsAt(doc, net, nodeId);
    for (const nodeId of order) this.crossingsAt(doc, net, graph, nodeId);
    for (const segId of [...roads].sort(ascending)) this.footwaysOf(doc, net, segId);
  }

// src/sim/peds/sidewalk.ts:101-139 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt (shared): the feature's statements; "…" marks code left out
  private kerbsAt(doc: RoadDoc, net: Network, nodeId: NodeId): void {
    // … 5 lines
    for (const segId of node.incident) {
      // … 2 lines
      const rt = roadProfile(seg.type, seg.lanes, seg.direction, seg.section, seg.parking);
      if (!carriesPedestrians(rt)) continue;
      // … 3 lines
      const lateral = rt.width / 2 + rt.sidewalk * 0.5;
      const want = kerbDistance(
        net.crosswalkDistanceAt(segId, nodeId),
        pl.length,
        net.mouthDistance(segId, nodeId),
      );
      const crossS = clearOfJunction(net, nodeId, pl, want, lateral, pl.length);
      const frame = pl.sampleAt(crossS);
      const nrm = perp(frame.t);
      // … 1 lines
      for (const side of [-1, 1] as const) {
        // … 3 lines
        this.nodes.set(id, {
          // … 1 lines
          at: addScaled(frame.p, nrm, (rt.width / 2 + footwayOn(rt, roadSide) * 0.5) * side),
        // … 3 lines
        });
      // … 1 lines
      }
    }
  }

// src/sim/peds/sidewalk.ts:142-174 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt (shared): the feature's statements; "…" marks code left out
  private crossingsAt(doc: RoadDoc, net: Network, graph: LaneletGraph, nodeId: NodeId): void {
    // … 3 lines
    for (const segId of node.incident) {
      // … 1 lines
      if (!segment || !carriesPedestrians(roadProfile(segment.type, segment.lanes, segment.direction, segment.section, segment.parking))) continue;
      if (net.crosswalkDistanceAt(segId, nodeId) <= 0) continue;
      // … 7 lines
      const lanes = graph.linksOfSegment(segId).slice();
      // … 1 lines
      this.addEdge({
        // … 9 lines
        lanes,
      });
    // … 2 lines
    }
  }

// src/sim/peds/sidewalk.ts:177-227 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.footwaysOf (shared): the feature's statements; "…" marks code left out
  private footwaysOf(doc: RoadDoc, net: Network, segId: SegmentId): void {
    // … 3 lines
    const rt = roadProfile(seg.type, seg.lanes, seg.direction, seg.section, seg.parking);
    if (!carriesPedestrians(rt)) return;
    // … 3 lines
    const s0 = kerbDistance(
      net.crosswalkDistanceAt(segId, seg.a),
      pl.length,
      net.mouthDistance(segId, seg.a),
    );
    const s1 = Math.max(
      s0 + 0.5,
      pl.length - kerbDistance(
        net.crosswalkDistanceAt(segId, seg.b),
        pl.length,
        net.mouthDistance(segId, seg.b),
      ),
    );
    const middle = pl.sub(s0, Math.min(s1, pl.length)).toPoints();
    // … 1 lines
    for (const side of [-1, 1] as const) {
      // … 5 lines
      const sideLateral = rt.width / 2 + footwayOn(rt, side > 0 ? 'left' : 'right') * 0.5;
      const offsetPts = middle.map((p, i) => {
        const t = pl.sampleAt(s0 + ((s1 - s0) * i) / Math.max(1, middle.length - 1)).t;
        return addScaled(p, perp(t), sideLateral * side);
      });
      // … 1 lines
      for (const point of [from.at, ...offsetPts, to.at]) {
        if (!points.length || dist(points[points.length - 1]!, point) > 0.01) points.push(point);
      }
    // … 10 lines
    }
  }

// src/sim/peds/sidewalk.ts:280-305 slice of src/sim/peds/sidewalk.ts#clearOfJunction (shared): the feature's statements; "…" marks code left out
function clearOfJunction(
  net: Network,
// … 5 lines
): number {
  const junction = net.junctionAt(node, Level.Asphalt);
  if (!junction) return want;
  // … 1 lines
  const surface = junction.rings.map((r) => r.flatten());
  // … 3 lines
  for (let attempt = 0; attempt < 14 && s < cap; attempt++) {
    // … 2 lines
    const inside = ([-1, 1] as const).some((side) =>
      surface.some((poly) => pointInPolygon(addScaled(frame.p, nrm, lateral * side), poly)),
    );
    if (!inside) return s;
  // … 1 lines
  }
// … 1 lines
}
