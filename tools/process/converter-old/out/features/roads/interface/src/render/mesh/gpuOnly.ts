// What the feature uses of src/render/mesh/gpuOnly.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/mesh/gpuOnly.ts:6-6 type GeometryArrays
/** DO NOT keep a mesh's arrays for a later rebuild: rebuild from the data it was made from. */

/** Each attribute's array by the attribute's name, and the index's under `index`. */
export type GeometryArrays = Readonly<Record<string, ArrayLike<number> | undefined>>;

// src/render/mesh/gpuOnly.ts:37-42 function fillArrays
/** A GPU-only mesh given arrays made elsewhere (the stored tiles, a worker) in place of its own source's; false where the mesh is not GPU-only. */
export function fillArrays(holder: Object3D, arrays: GeometryArrays): boolean;

// src/render/mesh/gpuOnly.ts:45-51 function gpuOnly
/** `gpuOnlyAttributes` for every attribute and the index of a geometry the mesh owns. */
export function gpuOnly(mesh: Object3D & { geometry: BufferGeometry }, arrays: () => GeometryArrays): void;

// src/render/mesh/gpuOnly.ts:54-56 function refillArrays
/** Every GPU-only mesh under `root` given its arrays again (a WebGL context restored). */
export function refillArrays(root: Object3D): void;
