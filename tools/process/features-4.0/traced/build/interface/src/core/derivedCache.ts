// What the feature uses of src/core/derivedCache.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/derivedCache.ts:3-3 variable STORE
const STORE = 'entries';

// src/core/derivedCache.ts:5-5 variable RELAXED
/** DO NOT wait for the disk on a derived write (MDN IDBDatabase.transaction, `durability: 'relaxed'`): what is kept here is made again if lost, and each commit held a tile's maker lane while the store caught up (the opening's workers at half load, 2026-10-17). The player's saves and the tiles (`editor/saveStore.ts`) keep the default. */
const RELAXED: IDBTransactionOptions = Object.freeze({ durability: 'relaxed' });

// src/core/derivedCache.ts:9-9 variable opened
/** The database once open (`database`): what a write that must clone its value now reads (`keepDerivedNow`). */
let opened: IDBDatabase | null = null;

// src/core/derivedCache.ts:11-42 function database
function database(): Promise<IDBDatabase | null>;

// src/core/derivedCache.ts:50-50 function prefixRange
/** The keys from `prefix` up to every key that starts with it. */
const prefixRange = (prefix: string): IDBKeyRange => …;

// src/core/derivedCache.ts:53-65 function readDerived
/** One entry, or undefined when it is not kept. */
export async function readDerived<T>(key: string): Promise<T | undefined>;

// src/core/derivedCache.ts:68-81 function readDerivedKeys
/** The keys that start with `prefix`, without it: what is kept, never the values (a store that grows with the places visited). */
export async function readDerivedKeys(prefix: string): Promise<Set<string>>;

// src/core/derivedCache.ts:142-158 function keepDerivedDurably
/** As `keepDerived`, and also tells when the write has committed (the transaction's `complete`, MDN) or failed */
export async function keepDerivedDurably(key: string, value: unknown): Promise<{ committed: Promise<void> }>;

// src/core/derivedCache.ts:250-252 function writeDerived
/** Keeps a value under `key`, without waiting. */
export function writeDerived(key: string, value: unknown): void;

// src/core/derivedCache.ts:327-341 function forgetOtherDerived
/** Forgets the entries of `kind` kept under any other fingerprint (an older build's). */
export function forgetOtherDerived(kind: string, fingerprint: string): void;
