// What the feature uses of src/render/people/lodWorker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/lodWorker.ts:6-14 interface LodRequest
/** The coarser levels of a person, simplified off the main thread */
export interface LodRequest {
  readonly id: number;
  readonly positions: Float32Array;
  readonly colours: Float32Array;
  readonly index: Uint32Array;
  readonly ranges: readonly { start: number; count: number }[];
  /** Per level: share of triangles kept, error allowed, and whether the seams between garments may move (`loose`). */
  readonly levels: readonly (readonly [number, number, boolean?])[];
}

// src/render/people/lodWorker.ts:17-23 interface IndexRequest
/** One index simplified to about `triangles` triangles (`simplifyIndex`): a procedural crowd's piece (`crowdLod.ts`). */
export interface IndexRequest {
  readonly id: number;
  readonly kind: 'index';
  readonly positions: Float32Array;
  readonly index: Uint32Array;
  readonly triangles: number;
}

// src/render/people/lodWorker.ts:26-35 interface LadderRequest
/** One welded mesh simplified once per error (`ladderIndices`): a vehicle part's levels (`vehicleLevels.ts`). */
export interface LadderRequest {
  readonly id: number;
  readonly kind: 'ladder';
  readonly positions: Float32Array;
  readonly attributes: Float32Array;
  readonly weights: readonly number[];
  readonly index: Uint32Array;
  /** Absolute errors, in the mesh's own units, finest first. */
  readonly errors: readonly number[];
}

// src/render/people/lodWorker.ts:42-45 interface LodLevel
export interface LodLevel {
  readonly index: Uint32Array;
  readonly groups: { start: number; count: number; materialIndex: number }[];
}

// src/render/people/lodWorker.ts:68-79 function simplifyIndex
/** `source` (triangles over `positions`) simplified to about `triangles` triangles over the same vertices */
export function simplifyIndex(source: Uint32Array, positions: Float32Array, triangles: number): Uint32Array | null;
