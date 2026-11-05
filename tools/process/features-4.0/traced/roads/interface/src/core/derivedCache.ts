// What the feature uses of src/core/derivedCache.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/derivedCache.ts:84-104 function readDerivedSome
/** The entries of these keys (under `prefix`), in one transaction: only what a build will use. */
export async function readDerivedSome<T>(prefix: string, keys: readonly string[]): Promise<Map<string, T>>;

// src/core/derivedCache.ts:175-197 function keepDerivedQueuedMany
export async function keepDerivedQueuedMany(entries: readonly (readonly [string, unknown])[], budget: WriteBudget): Promise<void>;

// src/core/derivedCache.ts:200-232 class WriteBudget
/** THE BYTES OF WRITES A THREAD HAS HANDED INDEXEDDB AND NOT SEEN COMMITTED, held under `most` (the V8 cage, the defect that kills the runs): one per thread that writes the derived cache, made where its service is served (`cityTile.worker.ts`, `renderer.ts`). */
export class WriteBudget { … }

// src/core/derivedCache.ts:255-257 function writeDerivedMany
/** Keeps many values, each under `prefix` and its own key, in one transaction */
export function writeDerivedMany(prefix: string, values: ReadonlyMap<string, unknown>): void;

// src/core/derivedCache.ts:327-341 function forgetOtherDerived
/** Forgets the entries of `kind` kept under any other fingerprint (an older build's). */
export function forgetOtherDerived(kind: string, fingerprint: string): void;
