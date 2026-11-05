// What the feature uses of src/core/forkMap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/forkMap.ts:2-107 class ForkMap
/** A COPY-ON-WRITE MAP OVER ANOTHER: a working copy of a keyed collection that costs what it writes, not what it holds. */
export class ForkMap<K, V> implements Map<K, V> { … }

// src/core/forkMap.ts:37-47 method ForkMap.writable
/** The value of `key` to change in place: the fork's own, or the one below copied (`copy`) and set here first. */
  writable(key: K, copy: (value: V) => V): V | undefined;

// src/core/forkMap.ts:95-97 method ForkMap.changes
/** What the fork wrote, in the order `applyTo` takes it: deleted keys, replaced values, added values. */
  changes(): { readonly gone: ReadonlySet<K>; readonly replaced: ReadonlyMap<K, V>; readonly added: ReadonlyMap<K, V> };

// src/core/forkMap.ts:100-106 method ForkMap.applyTo
/** Writes the fork's changes into `target`, a map holding what the map below held */
  applyTo(target: { set(key: K, value: V): unknown; delete(key: K): unknown },
    put: (key: K, value: V) => void = (key, value) => { target.set(key, value); },
    drop: (key: K) => void = (key) => { target.delete(key); }): void;

// src/core/forkMap.ts:175-175 variable forkWork
/** The work of the working copies , and the lists and indices a document's fork copies when it writes them: records */
export const forkWork = { copied: 0 };
