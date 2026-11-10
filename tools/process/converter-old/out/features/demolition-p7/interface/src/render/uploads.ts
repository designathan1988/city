// What the feature uses of src/render/uploads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/uploads.ts:17-23 function cancelUploads
/** A model released before its scheduled upload must never reach the GPU later. */
export function cancelUploads(textures: Iterable<Texture>): void;

// src/render/uploads.ts:75-98 function shadowStandIn
/** A caster as the shadow pass draws it: on its own depth material (or three's plain one) */
export function shadowStandIn(mesh: Mesh): Object3D | null;

// src/render/uploads.ts:242-245 function compileAhead
/** `shadow`: the object's material is a shadow's depth material, compiled for a shadow map. */
export function compileAhead(object: Object3D, shadow = false): Promise<void>;

// src/render/uploads.ts:440-443 function warmAhead
export function warmAhead(mesh: Mesh, geometries: readonly BufferGeometry[]): Promise<void>;
