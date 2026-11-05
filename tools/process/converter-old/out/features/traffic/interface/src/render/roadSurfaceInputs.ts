// What the feature uses of src/render/roadSurfaceInputs.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadSurfaceInputs.ts:93-139 method SurfaceInputs.refresh
/** @rebuild Brings the inputs up to `net`: the items its change since the last refresh names, and the roads round them, made again */
  refresh(net: Network): void;

// src/render/roadSurfaceInputs.ts:400-448 method SurfaceInputs.placeSegment
/** A road's inputs made again and filed. `silent`: derived again as it was (held round the view, `wakeTile`): no tile made dirty. */
  private placeSegment(net: Network, id: SegmentId, silent = false): void;
