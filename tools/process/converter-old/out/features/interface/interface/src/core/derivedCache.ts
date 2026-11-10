// What the feature uses of src/core/derivedCache.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/derivedCache.ts:142-158 function keepDerivedDurably
/** As `keepDerived`, and also tells when the write has committed (the transaction's `complete`, MDN) or failed */
export async function keepDerivedDurably(key: string, value: unknown): Promise<{ committed: Promise<void> }>;
