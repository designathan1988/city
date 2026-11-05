// What the feature uses of src/render/buildings/partPose.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/partPose.ts:6-6 variable POSE_FLOATS
/** A BUILDING PART'S PLACE AS THE GPU TAKES IT: a pose turned about the up axis - its place in three's axes, its turn's cosine and sine and its scale on three axes - 8 floats in place of a 16-float matrix; the vertex shader makes the matrix (as `cellInstances.ts` `posedMaterial` does) */

/** Floats a part's pose takes: x, y, z (three's axes), the turn's cosine; its sine, then the scale along the model's x, y and z. The turn kept as its cosine and sine, as the matrix kept it: the same numbers to the float. */
export const POSE_FLOATS = 8;

// src/render/buildings/partPose.ts:13-19 function poseMatrixInto
/** A pose's matrix, column-major T * Ry * S, as its GLSL makes it (`partPoseShader`). */
export function poseMatrixInto(poses: ArrayLike<number>, at: number, out: Float32Array | number[], outAt: number): void;
