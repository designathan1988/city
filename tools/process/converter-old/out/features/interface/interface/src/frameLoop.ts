// What the feature uses of src/frameLoop.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/frameLoop.ts:5-77 class FrameClock
/** THE FRAME LOOP'S BOOKKEEPING, out of `main.ts`: what the loop keeps between frames that is not the game's state. */

/** One frame asked for at a time, and the wall time between frames. */
export class FrameClock { … }

// src/frameLoop.ts:83-97 class Periodic
/** Work done every `seconds` of wall time, not every frame (the minimap ten times a second, the panels two and a half). */
export class Periodic { … }

// src/frameLoop.ts:100-115 class SimPeriodic
/** DO NOT drive the game's own periodic work with `setInterval`. */
export class SimPeriodic { … }
