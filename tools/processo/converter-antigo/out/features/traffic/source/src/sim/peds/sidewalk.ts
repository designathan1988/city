// The code of src/sim/peds/sidewalk.ts that is the feature's (the rest of the file is shared).

// src/sim/peds/sidewalk.ts:30-44 slice of src/sim/peds/sidewalk.ts#SidewalkEdge (shared): the feature's statements; "…" marks code left out
export interface SidewalkEdge {
  // … 10 lines
  readonly crossing?: CrossingId;
  // … 1 lines
  readonly lanes?: readonly LaneletId[];
}

// src/sim/peds/sidewalk.ts:50-50 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.crossings (shared): the feature's statements; "…" marks code left out
  readonly crossings = new Map<CrossingId, SidewalkEdgeId>();

// src/sim/peds/sidewalk.ts:55-55 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.ofNode (shared): the feature's statements; "…" marks code left out
  private readonly ofNode = new Map<NodeId, { readonly kerbs: SidewalkNodeId[]; readonly crossings: CrossingId[] }>();

// src/sim/peds/sidewalk.ts:58-63 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.build (shared): the feature's statements; "…" marks code left out
  build(doc: RoadDoc, net: Network, graph: LaneletGraph): void {
    // … 2 lines
    for (const nodeId of doc.nodes.keys()) this.crossingsAt(doc, net, graph, nodeId);
  // … 1 lines
  }

// src/sim/peds/sidewalk.ts:66-98 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.update (shared): the feature's statements; "…" marks code left out
  update(doc: RoadDoc, net: Network, graph: LaneletGraph, changes: readonly GraphChange[] | null): void {
    if (changes === null) { this.build(doc, net, graph); return; }
    // … 1 lines
    for (const change of changes) {
      for (const node of change.junctions) nodes.add(node);
      for (const seg of change.segments) roads.add(seg);
    }
    // … 23 lines
    for (const nodeId of order) this.crossingsAt(doc, net, graph, nodeId);
  // … 1 lines
  }

// src/sim/peds/sidewalk.ts:101-139 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.kerbsAt (shared): the feature's statements; "…" marks code left out
  private kerbsAt(doc: RoadDoc, net: Network, nodeId: NodeId): void {
    // … 3 lines
    const made = { kerbs: [] as SidewalkNodeId[], crossings: [] as CrossingId[] };
  // … 33 lines
  }

// src/sim/peds/sidewalk.ts:142-174 slice of src/sim/peds/sidewalk.ts#SidewalkGraph.crossingsAt (shared): the feature's statements; "…" marks code left out
  private crossingsAt(doc: RoadDoc, net: Network, graph: LaneletGraph, nodeId: NodeId): void {
    // … 3 lines
    for (const segId of node.incident) {
      // … 7 lines
      const crossing = makeCrossingId(nodeId, segId);
      // … 2 lines
      const lanes = graph.linksOfSegment(segId).slice();
      // … 1 lines
      this.addEdge({
        id: `X:${crossing}`,
        // … 7 lines
        crossing,
        lanes,
      });
      this.crossings.set(crossing, `X:${crossing}`);
      made.crossings.push(crossing);
    }
  }

// src/sim/peds/sidewalk.ts:239-241 method SidewalkGraph.madeAt: the feature's own
  madeAt(node: NodeId): object | undefined {
    return this.ofNode.get(node);
  }

// src/sim/peds/sidewalk.ts:243-246 method SidewalkGraph.crossingEdge: the feature's own
  crossingEdge(id: CrossingId): SidewalkEdge | undefined {
    const eid = this.crossings.get(id);
    return eid ? this.edges.get(eid) : undefined;
  }

// src/sim/peds/sidewalk.ts:249-251 method SidewalkGraph.walksBeside: the feature's own
  walksBeside(segment: SegmentId): readonly SidewalkEdge[] {
    return this.walksBySegment.get(segment) ?? [];
  }
