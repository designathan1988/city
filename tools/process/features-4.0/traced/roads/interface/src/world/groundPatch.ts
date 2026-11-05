// What the feature uses of src/world/groundPatch.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/groundPatch.ts:30-38 interface GroundPatch
/** The ground's corners over a rectangle: columns `i0`.., rows `j0`.. (southwards), `cols` × `rows` corners. */
export interface GroundPatch {
  readonly cell: number;
  readonly i0: number;
  readonly j0: number;
  readonly cols: number;
  readonly rows: number;
  /** Row by row, `cols` a row. */
  readonly heights: Float64Array;
}
