// What the feature uses of src/render/mesh/gpuOnly.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/gpuOnly.ts:6-6 type GeometryArrays
/** DO NOT keep a mesh's arrays for a later rebuild: rebuild from the data it was made from. */

/** Each attribute's array by the attribute's name, and the index's under `index`. */
export type GeometryArrays = Readonly<Record<string, ArrayLike<number> | undefined>>;
