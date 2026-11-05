// The code of src/world/viewCells.ts that is the feature's (the rest of the file is shared).

// src/world/viewCells.ts:173-183 method ViewCells.ensureMany: the feature's own
  ensureMany(keys: Iterable<CellKey>): void {
    const asked = new Set(keys);
    let made = false;
    for (const key of asked) {
      const known = this.held.get(key);
      if (known && known.version === this.spec.version) { this.touch(key, known); continue; }
      this.make(key);
      made = true;
    }
    if (made) this.trim(asked);
  }

// src/world/viewCells.ts:233-239 method ViewCells.forget: the feature's own
  forget(keys: Iterable<CellKey>): number {
    let n = 0;
    for (const key of keys) if (this.held.has(key)) { this.drop(key); n++; }
    this.work.invalidated += n;
    if (n) this.version++;
    return n;
  }
