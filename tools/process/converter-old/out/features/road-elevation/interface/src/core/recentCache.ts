// What the feature uses of src/core/recentCache.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/recentCache.ts:6-34 class RecentCache
/**
 * THE MOST RECENTLY ASKED VALUES, A BOUNDED NUMBER OF THEM: what is made again from what its owner keeps (a junction's outer levels,
 * a road's rings) held while it is asked, the longest unasked let go past `capacity` (a Map's insertion order is its age: MDN "Map",
 * "iteration happens in insertion order"). DO NOT keep in one anything that cannot be made again, the same, from its key alone.
 */
export class RecentCache<K, V> { … }

// src/core/recentCache.ts:12-24 method RecentCache.get
/** The value of `key`: the one held, moved to the newest, or made by `make` and held. */
  get(key: K, make: (key: K) => V): V;
