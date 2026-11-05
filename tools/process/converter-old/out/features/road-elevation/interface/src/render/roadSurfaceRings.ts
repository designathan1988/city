// What the feature uses of src/render/roadSurfaceRings.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/roadSurfaceRings.ts:13-20 interface Input
/** The rings the road surfaces are cut from, one at a time (`roadSurfaceInputs.ts`). */

/** One ring of input, with its box and its digest, measured once when its item is made. */
export interface Input {
  readonly poly: Poly;
  readonly minX: number;
  readonly minY: number;
  readonly maxX: number;
  readonly maxY: number;
  readonly digest: number;
}

// src/render/roadSurfaceRings.ts:22-43 function inputOf
export function inputOf(poly: Poly): Input | null;
