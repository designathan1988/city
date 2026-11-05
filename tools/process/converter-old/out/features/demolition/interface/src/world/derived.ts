// What the feature uses of src/world/derived.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/derived.ts:60-65 interface DerivedWork
/** What a product did: computes and whole rebuilds, since it was made (the ledger reads these). */
export interface DerivedWork {
  computes: number;
  wholes: number;
  /** Computes of the last refresh. */
  last: number;
}

// src/world/derived.ts:71-120 class Derived
/** A product derived from the document, key by key. */
export class Derived<D extends { … }

// src/world/derived.ts:93-119 method Derived.refresh
/** Computes what the changes since the last refresh made dirty, and what `more` does (entries the caller adds; null, everything); returns the keys whose value changed. */
  refresh(source: D, more: readonly Change[] | null = NO_CHANGES): ReadonlySet<K>;
