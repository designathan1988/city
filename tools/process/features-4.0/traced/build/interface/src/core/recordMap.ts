// What the feature uses of src/core/recordMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/recordMap.ts:2-21 class RecordMap
/** A MAP THAT TELLS ITS WRITES: each key's value before a set or a delete, while someone listens (an edit being recorded). */
export class RecordMap<K, V> extends Map<K, V> { … }

// src/core/recordMap.ts:4-4 field RecordMap.before
/** Told a key's value before it is written (undefined: the key was not there); null: nobody listens. */
  before: ((key: K, was: V | undefined) => void) | null = null;
