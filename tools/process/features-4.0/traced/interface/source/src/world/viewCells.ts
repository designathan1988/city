// The code of src/world/viewCells.ts that is the feature's (the rest of the file is shared).

// src/world/viewCells.ts:292-314 slice of src/world/viewCells.ts#ViewCells.trim (shared): the feature's statements; "…" marks code left out
  trim(keep: ReadonlySet<CellKey> | null, cpuBudget = this.spec.cpuBudget): void {
    const gpuBudget = (this.spec.gpuBudget ?? Infinity) * ViewCells.gpuShare;
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
  // … 7 lines
  }
