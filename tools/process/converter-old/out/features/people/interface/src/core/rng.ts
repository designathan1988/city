// What the feature uses of src/core/rng.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/rng.ts:6-106 class Rng
/** Deterministic RNG: xoshiro128** seeded through splitmix32. */
export class Rng { … }

// src/core/rng.ts:57-59 method Rng.float
/** Uniform in [0, 1). */
  float(): number;

// src/core/rng.ts:62-64 method Rng.range
/** Uniform in [lo, hi). */
  range(lo: number, hi: number): number;

// src/core/rng.ts:67-69 method Rng.int
/** Uniform integer in [lo, hi]. */
  int(lo: number, hi: number): number;

// src/core/rng.ts:87-96 method Rng.weighted
weighted<T>(entries: readonly (readonly [T, number])[]): T;
