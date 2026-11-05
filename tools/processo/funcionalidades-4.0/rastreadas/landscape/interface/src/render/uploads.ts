// What the feature uses of src/render/uploads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/uploads.ts:8-14 function queueUpload
export function queueUpload(...textures: (Texture | null | undefined)[]): void;

// src/render/uploads.ts:17-23 function cancelUploads
/** A model released before its scheduled upload must never reach the GPU later. */
export function cancelUploads(textures: Iterable<Texture>): void;

// src/render/uploads.ts:75-98 function shadowStandIn
/** A caster as the shadow pass draws it: on its own depth material (or three's plain one) */
export function shadowStandIn(mesh: Mesh): Object3D | null;

// src/render/uploads.ts:242-245 function compileAhead
/** `shadow`: the object's material is a shadow's depth material, compiled for a shadow map. */
export function compileAhead(object: Object3D, shadow = false): Promise<void>;

// src/render/uploads.ts:445-453 function warmLooseAhead
/** Meshes not in the scene yet (a batch of buildings put together to replace the one drawn); `priority`: the pixels of error their absence leaves on the screen, the largest sent first. Settles once their buffers are on the GPU and their programs, lit and in the shadow, are built (`programsReady`). */
export function warmLooseAhead(meshes: readonly Mesh[], priority: () => number = () => 0): Promise<void> | null;

// src/render/uploads.ts:515-525 function programsReady
/** DO NOT let a mesh be drawn before its programs are built (a program first used in the frame's draw stalls it, three's onFirstUse): its own and its shadow's, compiled ahead at the next drain (`drainCompiles`, KHR_parallel_shader_compile); what waits for the mesh waits for them too. */
export function programsReady(mesh: Mesh): Promise<unknown>;

// src/render/uploads.ts:590-592 function compilesAhead
/** Whether programs are built ahead here (a renderer answers; none in tests): a mesh waits for them only then. */
export function compilesAhead(): boolean;

// src/render/uploads.ts:659-664 function fullScreenTriangle
/** The geometry three's FullScreenQuad draws (`postprocessing/Pass.js`): one triangle with a position and a uv. */
export function fullScreenTriangle(): BufferGeometry;
