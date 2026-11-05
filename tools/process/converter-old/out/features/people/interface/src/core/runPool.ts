// What the feature uses of src/core/runPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/runPool.ts:2-74 class RunPool
/** RUNS OF NUMBERS BY KEY IN ONE TYPED ARRAY: each key's run (its start and length in columns at the key) in a shared `Float64Array`; a run let go stays until the dead outweigh the live (`pack`). No array a key, outside the JS heap (`kerbBays.ts` EntranceBook's runs, made general). */
export class RunPool { … }

// src/core/runPool.ts:6-6 field RunPool.data
/** The numbers of every run; DO NOT keep it across a `put`: packing makes another. */
  data = new Float64Array(64);

// src/core/runPool.ts:11-13 method RunPool.start
/** Where a key's run starts in `data`. */
  start(key: number): number;

// src/core/runPool.ts:16-18 method RunPool.length
/** How many numbers a key's run holds (0: none). */
  length(key: number): number;

// src/core/runPool.ts:21-32 method RunPool.put
/** Room for `n` numbers at a key, its old run let go: where to write them in `data` (read `data` after the call). */
  put(key: number, n: number): number;

// src/core/runPool.ts:43-47 method RunPool.drop
/** A key's run let go. */
  drop(key: number): void;

// src/core/runPool.ts:50-54 method RunPool.clear
/** Every run let go; the room kept. */
  clear(): void;
