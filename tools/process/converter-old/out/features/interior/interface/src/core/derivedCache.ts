// What the feature uses of src/core/derivedCache.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/derivedCache.ts:3-3 variable STORE
const STORE = 'entries';

// src/core/derivedCache.ts:11-42 function database
function database(): Promise<IDBDatabase | null>;

// src/core/derivedCache.ts:50-50 function prefixRange
/** The keys from `prefix` up to every key that starts with it. */
const prefixRange = (prefix: string): IDBKeyRange => …;

// src/core/derivedCache.ts:250-252 function writeDerived
/** Keeps a value under `key`, without waiting. */
export function writeDerived(key: string, value: unknown): void;

// src/core/derivedCache.ts:327-341 function forgetOtherDerived
/** Forgets the entries of `kind` kept under any other fingerprint (an older build's). */
export function forgetOtherDerived(kind: string, fingerprint: string): void;
