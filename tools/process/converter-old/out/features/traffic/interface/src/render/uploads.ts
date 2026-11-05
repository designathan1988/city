// What the feature uses of src/render/uploads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/uploads.ts:75-98 function shadowStandIn
/** A caster as the shadow pass draws it: on its own depth material (or three's plain one) */
export function shadowStandIn(mesh: Mesh): Object3D | null;

// src/render/uploads.ts:242-245 function compileAhead
/** `shadow`: the object's material is a shadow's depth material, compiled for a shadow map. */
export function compileAhead(object: Object3D, shadow = false): Promise<void>;
