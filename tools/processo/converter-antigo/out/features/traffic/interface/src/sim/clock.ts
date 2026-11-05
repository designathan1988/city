// What the feature uses of src/sim/clock.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/clock.ts:5-58 class SimClock
/** Fixed-timestep clock with an accumulator. */
export class SimClock { … }

// src/sim/clock.ts:7-7 field SimClock.tick
/** Written ONLY at the end of `pipeline.step`, so a step driven directly */
  tick = 0;

// src/sim/clock.ts:16-18 getter SimClock.time
get time(): number;

// src/sim/clock.ts:21-47 method SimClock.advance
/** Advances by wall-clock seconds, calling `step` once per fixed interval. */
  advance(wallDelta: number, step: () => void, budgetMs = Infinity, now: () => number = () => performance.now()): number;

// src/sim/clock.ts:62-65 function snapToTicks
function snapToTicks(seconds: number): number;
