// The code of src/render/roadSurfaceInputs.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfaceInputs.ts:451-531 slice of src/render/roadSurfaceInputs.ts#SurfaceInputs.placeNode (shared): the feature's statements; "…" marks code left out
  private placeNode(net: Network, node: NodeId, silent = false): void {
    // … 47 lines
    for (const r of rampsOfNode(net, node)) {
      if (footwayRiseShare(net, r.x + r.nx * r.run, r.y + r.ny * r.run) <= 0) continue;
    // … 1 lines
    }
  // … 28 lines
  }
