// What the feature uses of src/core/derivedCache.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/derivedCache.ts:53-65 function readDerived
/** One entry, or undefined when it is not kept. */
export async function readDerived<T>(key: string): Promise<T | undefined>;

// src/core/derivedCache.ts:84-104 function readDerivedSome
/** The entries of these keys (under `prefix`), in one transaction: only what a build will use. */
export async function readDerivedSome<T>(prefix: string, keys: readonly string[]): Promise<Map<string, T>>;

// src/core/derivedCache.ts:250-252 function writeDerived
/** Keeps a value under `key`, without waiting. */
export function writeDerived(key: string, value: unknown): void;

// src/core/derivedCache.ts:255-257 function writeDerivedMany
/** Keeps many values, each under `prefix` and its own key, in one transaction */
export function writeDerivedMany(prefix: string, values: ReadonlyMap<string, unknown>): void;
