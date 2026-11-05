// What the feature uses of src/render/mesh/gpuOnly.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/gpuOnly.ts:6-6 type GeometryArrays
/** DO NOT keep a mesh's arrays for a later rebuild: rebuild from the data it was made from. */

/** Each attribute's array by the attribute's name, and the index's under `index`. */
export type GeometryArrays = Readonly<Record<string, ArrayLike<number> | undefined>>;

// src/render/mesh/gpuOnly.ts:9-9 type Uploaded
/** What three uploads with an upload callback: an attribute or an interleaved buffer. */
type Uploaded = BufferAttribute | InterleavedBuffer;

// src/render/mesh/gpuOnly.ts:22-34 function gpuOnlyAttributes
/** Lets the arrays of `own` (a mesh's own attributes by name) go once each is on the GPU */
export function gpuOnlyAttributes(holder: Object3D, own: Readonly<Record<string, Uploaded>>, arrays: () => GeometryArrays): void;

// src/render/mesh/gpuOnly.ts:37-42 function fillArrays
/** A GPU-only mesh given arrays made elsewhere (the stored tiles, a worker) in place of its own source's; false where the mesh is not GPU-only. */
export function fillArrays(holder: Object3D, arrays: GeometryArrays): boolean;
