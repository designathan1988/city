// What the feature uses of src/render/people/bodyPixels.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/bodyPixels.ts:4-19 function bodyPixels
/** A procedural class's body baked from its shape basis (`crowdBodies.ts`): on the page or in the bake worker (`citizenBake.worker.ts`) */

/** A body's every vertex move from its shape coefficients `c`: the shape basis summed (`stride` texels, RGBA, as the bodies texture holds them). */
export function bodyPixels(shapePixels: Float32Array, vertexCount: number, shapes: number, c: Float32Array, stride: number): Float32Array;
