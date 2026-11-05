// What the feature uses of src/render/gpuLedger.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/gpuLedger.ts:7-7 type GpuClass
/** THE GPU'S BYTES IN ONE BOOK: every owner of video memory registers what it holds (bytes and triangles by class), and selection asks it before it requests (Cesium `cacheBytes`, 3DTilesRendererJS `lruCache.maxBytes`) */

/** What holds video memory, by class. */
export type GpuClass = 'building detail' | 'building facade' | 'cell masses' | 'tile masses' | 'plate masses' | 'base masses' | 'render targets' | 'shadow maps' | `far ${string}`;

// src/render/gpuLedger.ts:12-67 class GpuLedger
export class GpuLedger { … }

// src/render/gpuLedger.ts:15-15 field GpuLedger.sum
private sum = 0;

// src/render/gpuLedger.ts:17-17 field GpuLedger.cap
/** The most every owner together may hold, bytes (`deviceGpuCap`); Infinity until the renderer reads the device. */
  cap = Infinity;

// src/render/gpuLedger.ts:20-29 method GpuLedger.set
/** What `owner` holds now (replacing what it held), under `cls`. */
  set(owner: unknown, cls: GpuClass, bytes: number, triangles = 0): void;

// src/render/gpuLedger.ts:32-41 method GpuLedger.remove
/** `owner` let go of all it held. */
  remove(owner: unknown): void;

// src/render/gpuLedger.ts:44-46 method GpuLedger.bytes
/** Bytes held by a class, or by all. */
  bytes(cls?: GpuClass): number;

// src/render/gpuLedger.ts:70-70 variable gpuLedger
/** The game's one book of video memory. */
export const gpuLedger = new GpuLedger();

// src/render/gpuLedger.ts:73-85 function geometryCost
/** What a geometry sends to the GPU, bytes, and its triangles: read before its arrays are let go (`gpuOnly.ts` keeps `keptBytes`). */
export function geometryCost(geometry: BufferGeometry): { bytes: number; triangles: number };
