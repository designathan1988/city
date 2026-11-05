// What the feature uses of src/render/gpuLedger.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpuLedger.ts:73-85 function geometryCost
/** What a geometry sends to the GPU, bytes, and its triangles: read before its arrays are let go (`gpuOnly.ts` keeps `keptBytes`). */
export function geometryCost(geometry: BufferGeometry): { bytes: number; triangles: number };
