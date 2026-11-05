// What the feature uses of src/world/rasterGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/rasterGrid.ts:6-12 interface RasterGrid
/** A raster over the plane and the states of its cells, shared by the paved raster */

/** A raster: `W` x `H` cells of side `step` from (`minX`, `minY`); cell (i, j)'s centre is `minX + (i + 0.5) * step`, `minY + (j + 0.5) * step`. */
export interface RasterGrid {
  readonly minX: number;
  readonly minY: number;
  readonly W: number;
  readonly H: number;
  readonly step: number;
}

// src/world/rasterGrid.ts:17-17 variable PAVED
/** A raster cell's state: paved. */
export const PAVED = -2;

// src/world/rasterGrid.ts:19-19 variable NEAR
/** A raster cell's state: within a street's reach, to be asked whether paved (`markPaved` turns it to `PAVED` or `LAND`). */
export const NEAR = -3;

// src/world/rasterGrid.ts:22-22 function gridRect
/** The rectangle holding every cell centre of a raster. */
export const gridRect = (g: RasterGrid): ChangeRect => …;
