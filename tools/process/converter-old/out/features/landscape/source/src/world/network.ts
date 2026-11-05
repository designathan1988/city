// The code of src/world/network.ts that is the feature's (the rest of the file is shared).

// src/world/network.ts:233-249 method Network.crossingLandingAt: the feature's own
  crossingLandingAt(seg: SegmentId, node: NodeId): CrossingLanding | null {
    const kept = this.crossingLandings.get(seg)?.get(node);
    if (kept !== undefined) return kept;
    let landing: CrossingLanding | null = null;
    const crossing = this.crosswalkDistanceAt(seg, node);
    if (crossing > 0) {
      const segment = this.doc.requireSegment(seg);
      const road = roadProfile(segment.type, segment.lanes, segment.direction, segment.section, segment.parking);
      const frame = orientedPolyline(this.doc, segment, node).sampleAt(crossing);
      landing = { x: frame.p.x, y: frame.p.y, tx: frame.t.x, ty: frame.t.y,
        across: road.width / 2 + road.sidewalk + m(0.3), structure: segment.structure };
    }
    let byNode = this.crossingLandings.get(seg);
    if (!byNode) this.crossingLandings.set(seg, byNode = new Map());
    byNode.set(node, landing);
    return landing;
  }
