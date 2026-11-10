// What the feature uses of src/lab/tests/measure.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/tests/measure.ts:22-22 interface Cancel
/** Asked to stop: every wait looks at it. */
export interface Cancel { stopped: boolean }

// src/lab/tests/measure.ts:106-108 interface Hold
/** A frame asked for every refresh while a test runs (the loop draws on demand), and the clock held (`__roadcraft.measure`) when asked. */
export interface Hold {
  release(): void;
}

// src/lab/tests/measure.ts:109-129 function holdScene
export function holdScene(ctx: LabContext, clock: 'held' | 'running'): Hold;
