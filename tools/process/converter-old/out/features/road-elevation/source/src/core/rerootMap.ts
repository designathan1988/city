// The code of src/core/rerootMap.ts that is the feature's (the rest of the file is shared).

// src/core/rerootMap.ts:15-15 field RerootMap.prev: the feature's own
  private prev: RerootMap<K, V> | null = null;

// src/core/rerootMap.ts:16-16 field RerootMap.sealed: the feature's own
  private sealed = false;

// src/core/rerootMap.ts:19-27 slice of src/core/rerootMap.ts#RerootMap.get (shared): the feature's statements; "…" marks code left out
  get(key: K): V | undefined {
    // … 1 lines
    while (at.up) {
      const value = at.data.get(key);
      if (value !== undefined) return value === GONE ? undefined : value;
      at = at.up;
    }
    return at.data.get(key) as V | undefined;
  }

// src/core/rerootMap.ts:30-39 method RerootMap.derive: the feature's own
  derive(): RerootMap<K, V> {
    if (!this.sealed) throw new Error('RerootMap: a version is derived from once it is sealed');
    this.reroot();
    const next = new RerootMap<K, V>();
    next.data = this.data;
    next.prev = this;
    this.data = new Map();
    this.up = next;
    return next;
  }

// src/core/rerootMap.ts:41-44 method RerootMap.set: the feature's own
  set(key: K, value: V): void {
    this.note(key);
    this.data.set(key, value);
  }

// src/core/rerootMap.ts:46-50 method RerootMap.delete: the feature's own
  delete(key: K): void {
    if (!this.writable().data.has(key)) return;
    this.note(key);
    this.data.delete(key);
  }

// src/core/rerootMap.ts:53-56 method RerootMap.seal: the feature's own
  seal(): void {
    this.sealed = true;
    this.prev = null;
  }

// src/core/rerootMap.ts:59-70 method RerootMap.entries: the feature's own
  *entries(): IterableIterator<[K, V]> {
    const seen = new Set<K>();
    let at: RerootMap<K, V> = this;
    for (; at.up; at = at.up) {
      for (const [key, value] of at.data) {
        if (seen.has(key)) continue;
        seen.add(key);
        if (value !== GONE) yield [key, value];
      }
    }
    for (const [key, value] of at.data) if (!seen.has(key)) yield [key, value as V];
  }

// src/core/rerootMap.ts:72-75 method RerootMap.writable: the feature's own
  private writable(): this {
    if (this.up || this.sealed) throw new Error('RerootMap: only a version being made may be written');
    return this;
  }

// src/core/rerootMap.ts:78-83 method RerootMap.note: the feature's own
  private note(key: K): void {
    const prev = this.writable().prev;
    if (!prev || prev.data.has(key)) return;
    const value = this.data.get(key);
    prev.data.set(key, value === undefined ? GONE : value);
  }

// src/core/rerootMap.ts:86-103 method RerootMap.reroot: the feature's own
  private reroot(): void {
    if (!this.up) return;
    const path: RerootMap<K, V>[] = [];
    for (let at: RerootMap<K, V> = this; at.up; at = at.up) path.push(at);
    for (let i = path.length - 1; i >= 0; i--) {
      const layer = path[i]!, root = layer.up!;
      const back = new Map<K, Slot<V>>();
      for (const [key, value] of layer.data) {
        const now = root.data.get(key);
        back.set(key, now === undefined ? GONE : now);
        if (value === GONE) root.data.delete(key); else root.data.set(key, value);
      }
      layer.data = root.data;
      layer.up = null;
      root.data = back;
      root.up = layer;
    }
  }
