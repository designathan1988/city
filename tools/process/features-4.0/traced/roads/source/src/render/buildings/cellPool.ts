// The code of src/render/buildings/cellPool.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/cellPool.ts:34-38 slice of src/render/buildings/cellPool.ts#CellEnv (shared): the feature's statements; "…" marks code left out
export interface CellEnv {
  // … 2 lines
  readonly pavedAt?: PavingSource;
}

// src/render/buildings/cellPool.ts:259-263 slice of src/render/buildings/cellPool.ts#CellPool.offPage (shared): the feature's statements; "…" marks code left out
  private offPage(env: CellEnv): boolean {
    // … 2 lines
    return env.pavedAt === undefined || env.pavedAt.pack !== undefined;
  }

// src/render/buildings/cellPool.ts:282-295 slice of src/render/buildings/cellPool.ts#CellPool.write (shared): the feature's statements; "…" marks code left out
  private write(asked: Asked, sharesMemory: boolean): { data: CellJob; transfer: ArrayBuffer[] } {
    // … 4 lines
    const packed = asked.env.pavedAt && asked.rect ? asked.env.pavedAt.pack!(asked.rect) : null;
    if (asked.env.pavedAt && asked.rect && !packed) throw new Error('[buildings] the paving cannot be handed to a worker');
    const job: CellJob = {
      // … 2 lines
      ...(packed ? { paving: packed.data } : {}),
    };
    return { data: job, transfer: packed?.transfer ?? [] };
  }
