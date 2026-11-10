// The code of src/world/surfaces.ts that is the feature's (the rest of the file is shared).

// src/world/surfaces.ts:21-30 slice of src/world/surfaces.ts#SurfaceNet (shared): the feature's statements; "…" marks code left out
export interface SurfaceNet {
  readonly doc: {
    node(id: NodeId): { readonly incident: readonly SegmentId[] } | undefined;
    segment(id: SegmentId): { readonly a: NodeId; readonly b: NodeId; readonly curve: CurveShape | null; readonly structure: RoadStructure } | undefined;
    degree(id: NodeId): number;
  };
// … 3 lines
}

// src/world/surfaces.ts:55-70 slice of src/world/surfaces.ts#ownsJunction (shared): the feature's statements; "…" marks code left out
function ownsJunction(net: SurfaceNet, nodeId: NodeId, include: SurfaceSegmentFilter): boolean {
  // … 4 lines
  for (const id of node.incident) {
    // … 2 lines
    const clearance = roadStructure(structure).clearance;
    if (clearance < lowest) {
      lowest = clearance;
      owns = include(id);
    }
  }
  return owns;
}
