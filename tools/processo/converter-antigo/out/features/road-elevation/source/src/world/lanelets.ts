// The code of src/world/lanelets.ts that is the feature's (the rest of the file is shared).

// src/world/lanelets.ts:447-540 slice of src/world/lanelets.ts#LaneletGraph.buildSegmentLinks (shared): the feature's statements; "…" marks code left out
  private buildSegmentLinks(doc: RoadDoc, net: Network, segId: SegmentId, previous: Map<string, Lanelet[]>, next: Map<string, Lanelet[]>): void {
    // … 3 lines
    if (seg.structure === 'tunnel' && !TUNNELS_DRAWN) return;
  // … 88 lines
  }
