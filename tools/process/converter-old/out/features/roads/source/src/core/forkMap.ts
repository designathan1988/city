// The code of src/core/forkMap.ts that is the feature's (the rest of the file is shared).

// src/core/forkMap.ts:110-172 class ForkSet: the feature's own
export class ForkSet<K> implements Set<K> {
  private readonly map: ForkMap<K, true>;

  constructor(below: ReadonlySet<K>) {
    this.map = new ForkMap<K, true>({
      get: (key: K) => (below.has(key) ? true : undefined),
      has: (key: K) => below.has(key),
      get size() { return below.size; },
      *[Symbol.iterator]() { for (const key of below) yield [key, true] as [K, true]; },
      keys: () => below.keys(),
    } as unknown as ReadonlyMap<K, true>);
  }

  get [Symbol.toStringTag](): string {
    return 'Set';
  }

  get size(): number {
    return this.map.size;
  }

  has(key: K): boolean {
    return this.map.has(key);
  }

  add(key: K): this {
    if (!this.map.has(key)) this.map.set(key, true);
    return this;
  }

  delete(key: K): boolean {
    return this.map.delete(key);
  }

  clear(): void {
    this.map.clear();
  }

  *keys(): SetIterator<K> {
    yield* this.map.keys();
  }

  values(): SetIterator<K> {
    return this.keys();
  }

  *entries(): SetIterator<[K, K]> {
    for (const key of this.map.keys()) yield [key, key];
  }

  [Symbol.iterator](): SetIterator<K> {
    return this.keys();
  }

  forEach(visit: (value: K, key: K, set: Set<K>) => void, self?: unknown): void {
    for (const key of this.map.keys()) visit.call(self, key, key, this);
  }

  /** Writes the fork's changes into `target`, a set holding what the set below held. */
  applyTo(target: Set<K>): void {
    this.map.applyTo({ set: (key: K) => target.add(key), delete: (key: K) => target.delete(key) });
  }
}
