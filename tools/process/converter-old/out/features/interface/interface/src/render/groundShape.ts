// What the feature uses of src/render/groundShape.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/groundShape.ts:14-28 interface PlateShapeAsk
/** THE GROUND'S CUT AND FILL WORKED OUT OFF THE PAGE (`groundShape.worker.ts`) */

/** One plate's part of a shaping pass: its corners asked, as its mesh stands them. */
export interface PlateShapeAsk {
  /** The plate's centre: its corners are in its own coordinates, the roads and platforms in the world's. */
  readonly cx: number;
  readonly cy: number;
  /** Corners a side. */
  readonly n: number;
  /** The regions shaped, four numbers each (x0, x1, y0, y1, corners, clamped to the plate), in the pass's order. */
  readonly regions: Int32Array;
  /** `whole`: `xy` and `natural` hold every corner of the plate */
  readonly whole: boolean;
  /** Each corner's x and y where the mesh stands it (`CornerGrid.x`, `y`). */
  readonly xy: Float64Array;
  /** Each corner's height before any road or platform shaped it. */
  readonly natural: Float64Array;
}

// src/render/groundShape.ts:38-41 interface PlateShapeAnswer
/** The corners a plate's pass moved, in the order the pass met them, and their new heights. */
export interface PlateShapeAnswer {
  readonly corners: Int32Array;
  readonly heights: Float64Array;
}

// src/render/groundShape.ts:174-184 interface NaturalPlate
/** A plate's natural ground as `naturalRenderedHeightAt` reads it: its corners and the diagonals they are drawn with. */
export interface NaturalPlate {
  /** The plate's key (`world/bounds.ts` `plateOf`) and centre. */
  readonly index: number;
  readonly cx: number;
  readonly cy: number;
  readonly n: number;
  readonly cell: number;
  readonly half: number;
  readonly natural: Float64Array;
  readonly diagonals: Uint8Array;
}
