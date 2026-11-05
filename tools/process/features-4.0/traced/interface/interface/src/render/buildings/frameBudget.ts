// What the feature uses of src/render/buildings/frameBudget.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/buildings/frameBudget.ts:23-37 function letBackFitting
/** THE CELLS HELD BACK THAT FIT AGAIN (`layer.ts` capTriangles): each a level back, the ones spanning most of the screen first (`rank`, least first), while `drawn` with what it adds (`more`: far to its facade level, facade level to its detail) stays within `budget`; a far one comes back to its facade level only (held coarse), its detail when that fits too. DO NOT wait for the view to fall under `SLACK` of the budget: a still view never does, and the cells cut stayed cut (conferencia 40). */
export function letBackFitting(heldFar: Set<string>, heldCoarse: Set<string>, drawn: number, budget: number, rank: (key: string) => number, more: (key: string, far: boolean) => number): void;

// src/render/buildings/frameBudget.ts:41-54 function fitErrorScale
/** The scale that fits `budget`: `count(scale)` chooses the levels at that scale and says their triangles; `floor`, the least scale the frame's time allows (`layer.ts` `pressTime`).
 *  - Over the budget: coarser, step by step, until it fits or the coarsest is reached. Under `SLACK` of it and above the floor: one step finer, kept only if that still fits. */
export function fitErrorScale(scale: number, floor: number, budget: number, count: (scale: number) => number): number;

// src/render/buildings/frameBudget.ts:57-64 class FrameScale
/** THE SCALE OF A FRAME (`layer.ts` `levelOfDetail`): the most triangles the levels may add up to (`budget`, the view's, `core/perfBudget.ts`) and the scale that fits it (`scale`). The frame's time is the quality governor's (`render/quality.ts` QualityGovernor), never the levels': the detail is kept to 1.4 km. */
export class FrameScale { … }

// src/render/buildings/frameBudget.ts:61-63 method FrameScale.fit
/** The levels chosen at the scale that fits the budget: `count` chooses at a scale and says their triangles. */
  fit(count: (scale: number) => number): void;
