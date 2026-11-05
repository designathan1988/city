// What the feature uses of src/render/people/paletteTargets.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/people/paletteTargets.ts:7-7 variable PAGE_ROWS
/** THE PEOPLE'S BONE PALETTES IN COMMON TARGETS: every palette pass of the same width (`crowdAnimation.ts`) draws its rows into a page shared with the others, at its own rows */
/** DO NOT give each body class or cooked body a float target of its own: one target bound for them all, a range of rows each (its `base` added by the skinning). */

/** Rows of a page: what every WebGL2 device holds (MAX_TEXTURE_SIZE is at least 2048). A pass wanting more has a page of its own. */
const PAGE_ROWS = 2048;

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

// src/render/people/paletteTargets.ts:44-53 function fit
/** The page's target tall enough for its slabs: a new one, twice the height (its rows are drawn again by every pass the frame they are read). */
function fit(page: Page): void;

// src/render/people/paletteTargets.ts:56-72 function takeSlab
/** Rows for a pass `width` texels across: in a page of that width with room, or a new page. */
export function takeSlab(width: number, rows: number): Slab;

// src/render/people/paletteTargets.ts:75-82 function freeSlab
/** The pass's rows given back; an empty page's target let go. */
export function freeSlab(slab: Slab): void;
