// The code of src/render/roadSurfaceRings.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfaceRings.ts:54-58 slice of src/render/roadSurfaceRings.ts#medianOf.back (shared): the feature's statements; "…" marks code left out
  const back = (node: NodeId | undefined, from: { x: number; y: number }): number => {
    // … 1 lines
    const nose = node === undefined || !at ? 0 : medianNose(net, ribbon.id, node);
    return nose > 0 && at ? Math.max(0, nose - Math.hypot(from.x - at.x, from.y - at.y)) : 0;
  };
