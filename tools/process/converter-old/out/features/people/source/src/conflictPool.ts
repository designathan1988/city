// The code of src/conflictPool.ts that is the feature's (the rest of the file is shared).

// src/conflictPool.ts:184-200 slice of src/conflictPool.ts#ConflictPool.roundCorners (shared): the feature's statements; "…" marks code left out
  roundCorners(asks: readonly CornerAsk[]): Promise<readonly (readonly (Float64Array | null)[])[]> {
    // … 2 lines
    const per = Math.ceil(asks.length / lanes.length);
    // … 1 lines
    for (let k = 0; k < lanes.length && k * per < asks.length; k++) {
      const share = asks.slice(k * per, (k + 1) * per);
      const lane = lanes[k]!;
      const id = this.nextId++;
      parts.push(new Promise<(Float64Array | null)[][]>((resolve, reject) => {
        lane.waiting.set(id, { resolve: (answer) => resolve(answer as (Float64Array | null)[][]), reject });
        lane.worker.postMessage({ kind: 'corners', id, asks: share });
        this.arm(lane);
      }));
    }
  // … 1 lines
  }
