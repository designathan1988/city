// What the feature uses of src/core/recordMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/recordMap.ts:2-21 class RecordMap
/** A MAP THAT TELLS ITS WRITES: each key's value before a set or a delete, while someone listens (an edit being recorded). */
export class RecordMap<K, V> extends Map<K, V> { … }
