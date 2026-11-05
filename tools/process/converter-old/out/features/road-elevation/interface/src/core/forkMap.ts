// What the feature uses of src/core/forkMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/forkMap.ts:2-107 class ForkMap
/** A COPY-ON-WRITE MAP OVER ANOTHER: a working copy of a keyed collection that costs what it writes, not what it holds. */
export class ForkMap<K, V> implements Map<K, V> { … }

// src/core/forkMap.ts:20-25 method ForkMap.get
get(key: K): V | undefined;
