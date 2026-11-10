// What the feature uses of src/render/uploads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/uploads.ts:242-245 function compileAhead
/** `shadow`: the object's material is a shadow's depth material, compiled for a shadow map. */
export function compileAhead(object: Object3D, shadow = false): Promise<void>;

// src/render/uploads.ts:445-453 function warmLooseAhead
/** Meshes not in the scene yet (a batch of buildings put together to replace the one drawn); `priority`: the pixels of error their absence leaves on the screen, the largest sent first. Settles once their buffers are on the GPU and their programs, lit and in the shadow, are built (`programsReady`). */
export function warmLooseAhead(meshes: readonly Mesh[], priority: () => number = () => 0): Promise<void> | null;
