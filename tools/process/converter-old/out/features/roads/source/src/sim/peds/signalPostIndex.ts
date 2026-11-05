// The code of src/sim/peds/signalPostIndex.ts that is the feature's (the rest of the file is shared).

// src/sim/peds/signalPostIndex.ts:9-14 slice of src/sim/peds/signalPostIndex.ts#SignalPostWorld (shared): the feature's statements; "…" marks code left out
export interface SignalPostWorld {
  // … 1 lines
  readonly net: Network;
  readonly graph: LaneletGraph;
// … 1 lines
}

// src/sim/peds/signalPostIndex.ts:53-62 slice of src/sim/peds/signalPostIndex.ts#SignalPostIndex.placeJunction (shared): the feature's statements; "…" marks code left out
  private placeJunction(w: SignalPostWorld, node: NodeId): void {
    const junction = w.graph.junctions.get(node);
    // … 1 lines
    if (!junction?.signalised || !record) return;
    for (const segment of record.incident) {
      if (!junction.groups.some((group) => group.segments.includes(segment))) continue;
    // … 2 lines
    }
  }
