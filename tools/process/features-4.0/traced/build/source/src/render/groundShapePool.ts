// The code of src/render/groundShapePool.ts that is the feature's (the rest of the file is shared).

// src/render/groundShapePool.ts:21-30 slice of src/render/groundShapePool.ts#GroundWorkers (shared): the feature's statements; "…" marks code left out
interface GroundWorkers {
  // … 5 lines
  pads(ask: PadsAsk): Promise<(Pad | null)[]>;
// … 2 lines
}

// src/render/groundShapePool.ts:49-51 slice of src/render/groundShapePool.ts#GroundShapePool.pads (shared): the feature's statements; "…" marks code left out
  pads(ask: PadsAsk): Promise<(Pad | null)[]> {
    return this.run({ kind: 'pads', ask }) as Promise<(Pad | null)[]>;
  }
