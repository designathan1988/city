// What the feature uses of src/render/pinned.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/pinned.ts:5-9 function pinned
/** An object that never moves as an object (its instances move, or the shader places it), at the origin under parents at the origin: its matrices are the identity for good */
/** DO NOT leave three to compose and multiply its matrices every frame (`Object3D.updateMatrixWorld`: `compose` and `multiplyMatrices` at the top of the movement's profile). */
export function pinned<T extends Object3D>(object: T): T;
