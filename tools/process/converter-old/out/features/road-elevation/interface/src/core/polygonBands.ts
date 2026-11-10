// What the feature uses of src/core/polygonBands.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/polygonBands.ts:4-15 interface PolygonBands
/** A ring's edges sorted into horizontal bands: Haines' "bins" for the crossings test */
export interface PolygonBands {
  readonly y0: number;
  /** Bands per world unit. */
  readonly scale: number;
  readonly count: number;
  /** The ring's points, flat: x0, y0, x1, y1, ... */
  readonly xy: Float64Array;
  /** Band k's edges are `edge[start[k]]` up to `edge[start[k + 1]]`. */
  readonly start: Int32Array;
  /** An edge is named by its end point i; it runs from point i - 1 (n - 1 for 0). */
  readonly edge: Int32Array;
}
