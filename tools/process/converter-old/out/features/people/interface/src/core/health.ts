// What the feature uses of src/core/health.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/health.ts:132-136 interface FrameRecord
/** One frame's systems, ms each. */
export interface FrameRecord {
  readonly start: number;
  readonly end: number;
  readonly systems: ReadonlyMap<string, number>;
}

// src/core/health.ts:144-179 class FrameTimer
/** Times the systems of each frame: `begin` at the top of the frame, `mark` after each system with its name */
export class FrameTimer { … }
