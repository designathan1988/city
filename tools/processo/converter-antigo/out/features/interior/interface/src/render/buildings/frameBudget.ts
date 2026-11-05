// What the feature uses of src/render/buildings/frameBudget.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/frameBudget.ts:4-4 variable ERROR_STEP
/** THE ERROR SCALE THAT FITS THE FRAME: the levels are chosen by their error on the screen (`massCells.ts` `measureSeen`, 2 px at scale 1); when what they add up to passes the view's budget of triangles, the scale rises until it fits, and it comes back down a step at a time while there is room (Cesium's `maximumScreenSpaceError` adjusted to memory and time) */

/** Each step of the scale: a quarter coarser or finer. */
export const ERROR_STEP = 1.25;

// src/render/buildings/frameBudget.ts:6-6 variable MOST_ERROR_SCALE
/** The coarsest scale: 64 px of error, every cell at its masses or its plate (DO NOT climb for ever: a count that never fits, a view of nothing, would spin). */
export const MOST_ERROR_SCALE = 32;

// src/render/buildings/frameBudget.ts:8-8 variable SLACK
/** Under this share of the budget the scale may come down a step (a gap under the budget, so a step down does not step up again next frame). */
export const SLACK = 0.6;

// src/render/buildings/frameBudget.ts:58-58 field FrameScale.budget
budget = Infinity;

// src/render/buildings/frameBudget.ts:59-59 field FrameScale.scale
scale = 1;

// src/render/buildings/frameBudget.ts:67-67 variable TYPICAL_LEVEL_TRIANGLES
/** What a cell's level is reckoned to draw before any of its kind landed: a dense cell's detail and facade level with their parts, as measured by the predios session (2026-10-17: 1.3 M and 0.6 M, twice the 2026-10-16 figures). */
export const TYPICAL_LEVEL_TRIANGLES: Readonly<Record<'cell' | 'facade', number>> = { cell: 1_300_000, facade: 600_000 };
