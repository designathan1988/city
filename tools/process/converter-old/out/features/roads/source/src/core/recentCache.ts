// The code of src/core/recentCache.ts that is the feature's (the rest of the file is shared).

// src/core/recentCache.ts:6-34 class RecentCache: the feature's own
export class RecentCache<K, V> {
  private readonly held = new Map<K, V>();

  constructor(private readonly capacity: number) {}

  /** The value of `key`: the one held, moved to the newest, or made by `make` and held. */
  get(key: K, make: (key: K) => V): V {
    const held = this.held;
    const known = held.get(key);
    if (known !== undefined) {
      held.delete(key);
      held.set(key, known);
      return known;
    }
    const value = make(key);
    held.set(key, value);
    if (held.size > this.capacity) held.delete(held.keys().next().value as K);
    return value;
  }

  /** How many values are held. */
  get size(): number {
    return this.held.size;
  }

  clear(): void {
    this.held.clear();
  }
}
