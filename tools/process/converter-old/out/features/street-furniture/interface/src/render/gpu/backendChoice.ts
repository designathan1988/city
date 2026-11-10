// What the feature uses of src/render/gpu/backendChoice.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpu/backendChoice.ts:3-3 type GpuBackend
/** WHICH RENDERER DRAWS THE GAME (`MIGRACAO-WEBGPU.md`): WebGL, as always, unless `?gpu=webgpu` asks for the WebGPU renderer (the desktop's
 *  own setting adds it to the page's address, `desktop/main.cjs`). WebGPU is asked for only where the browser has it: elsewhere WebGL. */
export type GpuBackend = 'webgl' | 'webgpu';

// src/render/gpu/backendChoice.ts:5-8 function chosenBackend
export function chosenBackend(search: string = typeof location === 'undefined' ? '' : location.search,
  hasWebGpu: boolean = typeof navigator !== 'undefined' && 'gpu' in navigator): GpuBackend;
