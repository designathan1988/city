// The code of src/sim/peds/sidewalk.ts that is the feature's (the rest of the file is shared).

// src/sim/peds/sidewalk.ts:239-241 method SidewalkGraph.madeAt: the feature's own
  madeAt(node: NodeId): object | undefined {
    return this.ofNode.get(node);
  }

// src/sim/peds/sidewalk.ts:243-246 method SidewalkGraph.crossingEdge: the feature's own
  crossingEdge(id: CrossingId): SidewalkEdge | undefined {
    const eid = this.crossings.get(id);
    return eid ? this.edges.get(eid) : undefined;
  }
