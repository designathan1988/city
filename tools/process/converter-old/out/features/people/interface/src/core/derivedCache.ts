// What the feature uses of src/core/derivedCache.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/derivedCache.ts:53-65 function readDerived
/** One entry, or undefined when it is not kept. */
export async function readDerived<T>(key: string): Promise<T | undefined>;

// src/core/derivedCache.ts:250-252 function writeDerived
/** Keeps a value under `key`, without waiting. */
export function writeDerived(key: string, value: unknown): void;

// src/core/derivedCache.ts:327-341 function forgetOtherDerived
/** Forgets the entries of `kind` kept under any other fingerprint (an older build's). */
export function forgetOtherDerived(kind: string, fingerprint: string): void;
