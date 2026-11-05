// What the feature uses of src/core/rng.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/rng.ts:6-106 class Rng
/** Deterministic RNG: xoshiro128** seeded through splitmix32. */
export class Rng { … }

// src/core/rng.ts:7-7 field Rng.s0
private s0: number;

// src/core/rng.ts:8-8 field Rng.s1
private s1: number;

// src/core/rng.ts:9-9 field Rng.s2
private s2: number;

// src/core/rng.ts:10-10 field Rng.s3
private s3: number;

// src/core/rng.ts:57-59 method Rng.float
/** Uniform in [0, 1). */
  float(): number;

// src/core/rng.ts:67-69 method Rng.int
/** Uniform integer in [lo, hi]. */
  int(lo: number, hi: number): number;
