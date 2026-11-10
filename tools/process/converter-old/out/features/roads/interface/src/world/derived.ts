// What the feature uses of src/world/derived.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/derived.ts:6-37 class JournalCursor
/** DO NOT key a product on a global revision (`${doc.buildings.revision}`) and build the map again */

/** ONE WAY TO READ THE DIARY the cursor every reader shares, instead of fourteen hand-written  calls each with its own */
export class JournalCursor implements JournalReader { … }

// src/world/derived.ts:19-30 method JournalCursor.read
/** The entries of its kinds since the last read, oldest first; null: everything changed. */
  read(journal: ChangeJournal): readonly Change[] | null;

// src/world/derived.ts:40-57 interface DerivedSpec
/** What a product reads and how it is computed, key by key. */
export interface DerivedSpec<D, K, V> {
  /** For the ledger and the console. */
  readonly name: string;
  /** The kinds of change it reads; nothing else makes it compute. */
  readonly kinds: readonly ChangeKind[];
  /** Whether an entry without ids makes everything dirty (a product by item). */
  readonly byId: boolean;
  /** DO NOT walk the map anywhere else in a product: `dirty` and `compute` work on what one change touched. */
  allKeys(source: D): Iterable<K>;
  /** The keys one change makes dirty, added to `out`. */
  dirty(change: Change, out: Set<K>, source: D): void;
  /** The value of a key now; undefined: there is none (the item is gone). */
  compute(key: K, source: D): V | undefined;
  /** Whether two values are the same (Salsa's backdating); by identity when absent. */
  equals?(a: V, b: V): boolean;
  /** Told of each key whose value changed, for an index kept beside the values. */
  changed?(key: K, before: V | undefined, after: V | undefined): void;
}

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

// src/world/derived.ts:74-74 field Derived.work
readonly work: DerivedWork = { computes: 0, wholes: 0, last: 0 };

// src/world/derived.ts:80-82 method Derived.get
get(key: K): V | undefined;

// src/world/derived.ts:93-119 method Derived.refresh
/** Computes what the changes since the last refresh made dirty, and what `more` does (entries the caller adds; null, everything); returns the keys whose value changed. */
  refresh(source: D, more: readonly Change[] | null = NO_CHANGES): ReadonlySet<K>;

// src/world/derived.ts:128-187 class ChangeSweep
/** DO NOT ask the journal per reader per frame: each asked for every entry since its serial */
export class ChangeSweep implements JournalReader { … }
