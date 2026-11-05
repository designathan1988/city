// What the feature uses of src/render/uploads.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/uploads.ts:260-368 function drainCompiles
/** Starts compiling what is waiting; each promise settles when its shaders are ready. */
export function drainCompiles(renderer: WebGLRenderer, camera: Camera, scene: Scene, target: WebGLRenderTarget | null): void;

// src/render/uploads.ts:493-512 function drainWarm
/** Sends waiting meshes to the GPU after the frame (`renderer.ts`): the scene's first, then the loose ones by their error on the screen, up to `UPLOAD_BYTES` of buffers a frame (one at least, so the queue always moves). */
export function drainWarm(renderer: WebGLRenderer): void;

// src/render/uploads.ts:659-664 function fullScreenTriangle
/** The geometry three's FullScreenQuad draws (`postprocessing/Pass.js`): one triangle with a position and a uv. */
export function fullScreenTriangle(): BufferGeometry;
