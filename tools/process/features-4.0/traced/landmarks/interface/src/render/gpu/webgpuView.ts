// What the feature uses of src/render/gpu/webgpuView.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpu/webgpuView.ts:15-22 interface CompareOptions
/** THE WEBGPU RENDERER, STAGE 1 (`MIGRACAO-WEBGPU.md`): three's `WebGPURenderer` drawing the game's scene as it stands. What it can draw
 *  as is (three's own materials) and what has a node twin (`twins.ts`) is drawn; what it cannot (`compatibility.ts`) is hidden for its
 *  frame alone and named once in the console. The WebGL renderer keeps everything else of the game (programs, uploads, shadows' stores) on a
 *  canvas of its own: this module only draws. Loaded on demand: the WebGL game never loads it. */

export interface CompareOptions {
  /** A pixel differs when its largest channel differs by more than this, in 8-bit steps. */
  threshold?: number;
  /** Frames drawn by each renderer to time it, GPU ms (median); 0 for none. */
  timingFrames?: number;
  /** Leave out of both pictures what only a simple twin draws (`twins.ts` kind): both renderers then draw the same things, the same way. */
  faithfulOnly?: boolean;
}

// src/render/gpu/webgpuView.ts:24-40 interface CompareResult
export interface CompareResult {
  width: number;
  height: number;
  /** The two pictures and their difference, PNG data URLs. */
  webgl: string;
  webgpu: string;
  diff: string;
  difference: Omit<PictureDifference, 'image'>;
  /** Each renderer's GPU time drawing that view, ms: the median of `timingFrames` draws (null when the browser does not time it). */
  gpuMs: { webgl: number | null; webgpu: number | null };
  /** What both left out of the view (no WebGPU equivalent yet), drawables by reason. */
  leftOut: Record<string, number>;
  /** What the WebGPU picture drew with a node twin instead of the game's material: drawables by name and kind. */
  twins: Record<string, number>;
  /** What both drew as it is (three's own materials): how many drawables, and some names. */
  asIs: { count: number; names: string[] };
}
