// The code of src/render/roadSurfaces.ts that is the feature's (the rest of the file is shared).

// src/render/roadSurfaces.ts:283-794 slice of src/render/roadSurfaces.ts#roadSurfaceSteps (shared): the feature's statements; "…" marks code left out
export function* roadSurfaceSteps(
// … 10 lines
): Generator<'wait' | void, RoadSurfaces, void> {
  // … 40 lines
  for (const pass of passes) {
    // … 126 lines
    const dataFor = (rects: readonly TileRect[]): { data: SurfaceData; transfer: ArrayBuffer[] } => {
      // … 25 lines
      const flush = packFlush(net, around);
      transfer.push(...flush.transfer);
      // … 4 lines
      return { data: { field, looks: lookList, flush: flush.data, ramps: [...ramps].map(rampData), edges: edgeData }, transfer };
    };
  // … 264 lines
  }
// … 33 lines
}
