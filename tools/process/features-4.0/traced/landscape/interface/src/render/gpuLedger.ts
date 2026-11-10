// What the feature uses of src/render/gpuLedger.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpuLedger.ts:12-67 class GpuLedger
export class GpuLedger { … }

// src/render/gpuLedger.ts:70-70 variable gpuLedger
/** The game's one book of video memory. */
export const gpuLedger = new GpuLedger();

// src/render/gpuLedger.ts:73-85 function geometryCost
/** What a geometry sends to the GPU, bytes, and its triangles: read before its arrays are let go (`gpuOnly.ts` keeps `keptBytes`). */
export function geometryCost(geometry: BufferGeometry): { bytes: number; triangles: number };
