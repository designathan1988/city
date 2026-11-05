// What the feature uses of src/render/mesh/roadLodPure.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/roadLodPure.ts:6-13 type BlockArrays
/** THE FAR LEVEL OF A BLOCK OF ROAD SURFACE, as arrays (`roadLod.ts` makes the mesh) */

/** A block's arrays, as `tileArrays` makes them: `color` and `roadSection` only where the surface's material reads them. */
export type BlockArrays = {
  readonly position: Float32Array;
  readonly normal: Float32Array;
  readonly uv: Float32Array;
  readonly color?: Float32Array;
  readonly roadSection?: Float32Array;
  readonly index: Uint16Array | Uint32Array;
};
