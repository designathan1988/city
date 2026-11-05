// What the feature uses of src/core/derivedCache.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/derivedCache.ts:45-47 function derivedCacheOpen
/** Whether the store opens here (none in Node, refused in some private windows): what is kept can be read back. */
export async function derivedCacheOpen(): Promise<boolean>;

// src/core/derivedCache.ts:53-65 function readDerived
/** One entry, or undefined when it is not kept. */
export async function readDerived<T>(key: string): Promise<T | undefined>;

// src/core/derivedCache.ts:175-197 function keepDerivedQueuedMany
export async function keepDerivedQueuedMany(entries: readonly (readonly [string, unknown])[], budget: WriteBudget): Promise<void>;

// src/core/derivedCache.ts:200-232 class WriteBudget
/** THE BYTES OF WRITES A THREAD HAS HANDED INDEXEDDB AND NOT SEEN COMMITTED, held under `most` (the V8 cage, the defect that kills the runs): one per thread that writes the derived cache, made where its service is served (`cityTile.worker.ts`, `renderer.ts`). */
export class WriteBudget { … }

// src/core/derivedCache.ts:327-341 function forgetOtherDerived
/** Forgets the entries of `kind` kept under any other fingerprint (an older build's). */
export function forgetOtherDerived(kind: string, fingerprint: string): void;
