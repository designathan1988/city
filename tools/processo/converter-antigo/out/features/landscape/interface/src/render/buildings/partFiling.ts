// What the feature uses of src/render/buildings/partFiling.ts (shared with the program): declarations as the source writes them, bodies left out.

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
