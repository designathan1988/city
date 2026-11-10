// What the feature uses of src/render/buildings/partFiling.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/partFiling.ts:13-13 variable DETAIL_CELL
/** Side of the sub-cells a cell's parts are filed by (four a side): each part kind's level is chosen per sub-cell, or per quadrant or cell where they agree (`partCells.ts`). DO NOT read it of `layerCells.ts`: that module reaches the shells, which reach the filing. */
export const DETAIL_CELL = m(64);

// src/render/buildings/partFiling.ts:15-15 variable SIDE
/** Sub-cells a side of a cell (the world's one grid, `layerCells.ts` BATCH_CELL): four, as `morton` is made for. */
export const SIDE = Math.round(CELL / DETAIL_CELL);

// src/render/buildings/partFiling.ts:17-17 variable LEAVES
export const LEAVES = SIDE * SIDE;

// src/render/buildings/partFiling.ts:19-19 variable DRAW_TRIANGLES
/** What a draw is taken to cost in triangles: a band is kept apart only if it draws as much (three's per-object work on the page, `projectObject`/`renderObject`, outweighs this many triangles on the GPU). */
export const DRAW_TRIANGLES = 16384;

// src/render/buildings/partFiling.ts:33-46 interface FiledBand
/** One band of a kind's instances in the cell, by sub-cell in Morton order. */
export interface FiledBand {
  readonly kind: PartKind;
  /** Each sub-cell's first instance; the last entry is the band's count. */
  readonly start: Uint32Array;
  /** Each sub-cell's box in three's world (min x, y, z, max x, y, z). */
  readonly box: Float64Array;
  /** Each sub-cell's largest scale of an instance along each of the model's axes, then the cell's. */
  readonly scale: Float32Array;
  readonly poses: Float32Array;
  /** Tints in half floats, as the sub-cells' (`cellArrays.ts` `instanceColor`). */
  readonly colours: Uint16Array | null;
  /** The shell's parts: each one's finish, wear and ground (`shellParts.ts`). */
  readonly looks: Float32Array | null;
}

// src/render/buildings/partFiling.ts:49-51 interface PartFiling
/** A cell's parts, every kind's bands. */
export interface PartFiling {
  readonly bands: readonly FiledBand[];
}
