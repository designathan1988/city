// What the feature uses of src/core/rng.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/rng.ts:3-3 type RngState
/** The four words of an `Rng`'s state (`Rng.state`). */
export type RngState = readonly [number, number, number, number];

// src/core/rng.ts:6-106 class Rng
/** Deterministic RNG: xoshiro128** seeded through splitmix32. */
export class Rng { … }
