// The code of src/world/junction/build.ts that is the feature's (the rest of the file is shared).

// src/world/junction/build.ts:48-80 slice of src/world/junction/build.ts#surfaceMode (shared): the feature's statements; "…" marks code left out
export function surfaceMode(doc: RoadDoc, cache: PolylineCache, nodeId: NodeId): SurfaceMode {
  // … 21 lines
  if (node.heightOffset < -TUNNEL_HEADROOM) return 'none';
// … 9 lines
}
