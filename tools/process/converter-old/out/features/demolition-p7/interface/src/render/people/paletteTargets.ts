// What the feature uses of src/render/people/paletteTargets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/paletteTargets.ts:9-15 interface Page
interface Page {
  readonly width: number;
  target: WebGLRenderTarget;
  /** Rows the target has (grown by doubling, up to `PAGE_ROWS`, or more for a page of one pass). */
  height: number;
  readonly slabs: Slab[];
}

// src/render/people/paletteTargets.ts:18-22 interface Slab
/** One pass's rows in a page: [base, base + rows). */
export interface Slab {
  page: Page;
  base: number;
  rows: number;
}

// src/render/people/paletteTargets.ts:75-82 function freeSlab
/** The pass's rows given back; an empty page's target let go. */
export function freeSlab(slab: Slab): void;
