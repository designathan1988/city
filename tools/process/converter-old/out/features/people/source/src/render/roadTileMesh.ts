// The code of src/render/roadTileMesh.ts that is the feature's (the rest of the file is shared).

// src/render/roadTileMesh.ts:14-25 slice of src/render/roadTileMesh.ts#SurfaceData (shared): the feature's statements; "…" marks code left out
export interface SurfaceData {
  // … 5 lines
  readonly flush: FlushData;
// … 4 lines
}

// src/render/roadTileMesh.ts:38-74 slice of src/render/roadTileMesh.ts#readsOf (shared): the feature's statements; "…" marks code left out
function readsOf(data: SurfaceData, heights: PointHeights, missing: number[]): SurfaceReads {
  // … 16 lines
  const footwayShare = flushShareOf(data.flush);
  // … 9 lines
  return {
    // … 4 lines
    footwayShare,
  // … 2 lines
  };
}
