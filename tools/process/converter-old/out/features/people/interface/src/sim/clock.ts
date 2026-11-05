// What the feature uses of src/sim/clock.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/clock.ts:5-58 class SimClock
/** Fixed-timestep clock with an accumulator. */
export class SimClock { … }

// src/sim/clock.ts:7-7 field SimClock.tick
/** Written ONLY at the end of `pipeline.step`, so a step driven directly */
  tick = 0;
