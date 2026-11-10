// The code of src/core/ring.ts that is the feature's (the rest of the file is shared).

// src/core/ring.ts:55-57 method Ring.run: the feature's own
  run(): { readonly data: readonly number[]; readonly count: number } {
    return { data: this.data, count: this.count };
  }

// src/core/ring.ts:60-65 method Ring.fromRun: the feature's own
  static fromRun(run: { readonly data: readonly number[]; readonly count: number }): Ring {
    const ring = Object.create(Ring.prototype) as { data: readonly number[]; count: number };
    ring.data = run.data.slice();
    ring.count = run.count;
    return ring as unknown as Ring;
  }
