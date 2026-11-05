// The code of src/render/gpuLedger.ts that is the feature's (the rest of the file is shared).

// src/render/gpuLedger.ts:62-66 method GpuLedger.roomFor: the feature's own
  roomFor(classes: readonly GpuClass[]): number {
    let theirs = 0;
    for (const cls of classes) theirs += this.bytes(cls);
    return this.cap - (this.sum - theirs);
  }
