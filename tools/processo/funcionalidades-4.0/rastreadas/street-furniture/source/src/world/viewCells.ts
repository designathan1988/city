// The code of src/world/viewCells.ts that is the feature's (the rest of the file is shared).

// src/world/viewCells.ts:153-158 method ViewCells.peek: the feature's own
  peek(key: CellKey): V | undefined {
    const known = this.held.get(key);
    if (!known || known.version !== this.spec.version) return undefined;
    this.touch(key, known);
    return known.product.value;
  }

// src/world/viewCells.ts:210-220 method ViewCells.want: the feature's own
  want(keys: readonly CellKey[]): void {
    // Copied into its own list, kept from look to look (asked every frame of a move): never a new array each time.
    const wanted = this.wanted;
    // Its own list handed back (`wantedKeys`): nothing to copy, and emptying it first would empty what it reads.
    if (keys !== wanted) {
      wanted.length = 0;
      for (const key of keys) wanted.push(key);
    }
    this.wantedSet.clear();
    for (const key of this.wanted) this.wantedSet.add(key);
  }

// src/world/viewCells.ts:223-230 method ViewCells.account: the feature's own
  account(key: CellKey, cpuBytes: number, gpuBytes: number): void {
    const known = this.held.get(key);
    if (!known) return;
    this.cpuBytes += cpuBytes - known.product.cpuBytes;
    this.gpuBytes += gpuBytes - (known.product.gpuBytes ?? 0);
    known.product = { value: known.product.value, cpuBytes, gpuBytes };
    known.stage = gpuBytes > 0 ? CellStage.resident : CellStage.derived;
  }

// src/world/viewCells.ts:233-239 method ViewCells.forget: the feature's own
  forget(keys: Iterable<CellKey>): number {
    let n = 0;
    for (const key of keys) if (this.held.has(key)) { this.drop(key); n++; }
    this.work.invalidated += n;
    if (n) this.version++;
    return n;
  }

// src/world/viewCells.ts:292-314 slice of src/world/viewCells.ts#ViewCells.trim (shared): the feature's statements; "…" marks code left out
  trim(keep: ReadonlySet<CellKey> | null, cpuBudget = this.spec.cpuBudget): void {
    // … 1 lines
    if (this.gpuBytes > gpuBudget) {
      for (const [key, held] of this.held) {
        if (this.gpuBytes <= gpuBudget) break;
        if (keep?.has(key) || this.wantedSet.has(key) || !(held.product.gpuBytes ?? 0)) continue;
        if (this.spec.releaseGpu) {
          this.spec.releaseGpu(key, held.product.value);
          this.gpuBytes -= held.product.gpuBytes ?? 0;
          held.product = { value: held.product.value, cpuBytes: held.product.cpuBytes };
          held.stage = CellStage.derived;
        } else this.drop(key);
        this.work.evicted++;
      }
    }
    if (this.cpuBytes <= cpuBudget) return;
    for (const key of [...this.held.keys()]) {
      if (this.cpuBytes <= cpuBudget) break;
    // … 3 lines
    }
  }

// src/world/viewCells.ts:316-326 slice of src/world/viewCells.ts#ViewCells.make (shared): the feature's statements; "…" marks code left out
  private make(key: CellKey): V {
    // … 6 lines
    this.cpuBytes += product.cpuBytes + ENTRY_BYTES;
    this.gpuBytes += gpu;
  // … 1 lines
  }

// src/world/viewCells.ts:328-335 slice of src/world/viewCells.ts#ViewCells.drop (shared): the feature's statements; "…" marks code left out
  private drop(key: CellKey): void {
    // … 3 lines
    this.cpuBytes -= known.product.cpuBytes + ENTRY_BYTES;
    this.gpuBytes -= known.product.gpuBytes ?? 0;
  // … 1 lines
  }
