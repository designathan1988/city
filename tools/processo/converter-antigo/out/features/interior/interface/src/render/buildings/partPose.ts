// What the feature uses of src/render/buildings/partPose.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/partPose.ts:6-6 variable POSE_FLOATS
/** A BUILDING PART'S PLACE AS THE GPU TAKES IT: a pose turned about the up axis - its place in three's axes, its turn's cosine and sine and its scale on three axes - 8 floats in place of a 16-float matrix; the vertex shader makes the matrix (as `cellInstances.ts` `posedMaterial` does) */

/** Floats a part's pose takes: x, y, z (three's axes), the turn's cosine; its sine, then the scale along the model's x, y and z. The turn kept as its cosine and sine, as the matrix kept it: the same numbers to the float. */
export const POSE_FLOATS = 8;

// src/render/buildings/partPose.ts:29-41 function posedCopy
/** A model with a batch's poses as its instanced attributes, bound at instance `at`: the model's own attributes and index shared (three keeps one buffer per attribute, r186 `WebGLAttributes`). */
export function posedCopy(model: BufferGeometry, poses: InstancedInterleavedBuffer, at = 0): BufferGeometry;

// src/render/buildings/partPose.ts:44-48 function letGoPosed
/** A posed copy let go: the model's attributes and index taken off first, so three frees the poses' buffer only (r186 `WebGLGeometries` frees every attribute a disposed geometry holds). */
export function letGoPosed(g: BufferGeometry, model: BufferGeometry): void;

// src/render/buildings/partPose.ts:95-110 function partPoseDepth
/** The shadow pass's material of a part's material: three's depth material drawn from the poses, one per material (r186 `WebGLShadowMap` copies its map and alpha test onto it each draw). */
export function partPoseDepth(material: Material): MeshDepthMaterial;
