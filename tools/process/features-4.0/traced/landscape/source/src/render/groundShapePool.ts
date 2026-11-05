// The code of src/render/groundShapePool.ts that is the feature's (the rest of the file is shared).

// src/render/groundShapePool.ts:6-13 slice of src/render/groundShapePool.ts#Lane (shared): the feature's statements; "…" marks code left out
interface Lane {
  // … 1 lines
  readonly waiting: Map<number, { resolve(answer: GroundJobAnswer): void; reject(error: Error): void }>;
// … 4 lines
}

// src/render/groundShapePool.ts:21-30 slice of src/render/groundShapePool.ts#GroundWorkers (shared): the feature's statements; "…" marks code left out
interface GroundWorkers {
  // … 2 lines
  shape(ask: GroundShapeAsk): Promise<PlateShapeAnswer[]>;
  digest(ask: BlockDigestAsk): Promise<Float64Array[]>;
  // … 1 lines
  pads(ask: PadsAsk): Promise<(Pad | null)[]>;
// … 2 lines
}

// src/render/groundShapePool.ts:41-43 slice of src/render/groundShapePool.ts#GroundShapePool.shape (shared): the feature's statements; "…" marks code left out
  shape(ask: GroundShapeAsk): Promise<PlateShapeAnswer[]> {
    return this.run({ kind: 'shape', ask }) as Promise<PlateShapeAnswer[]>;
  }

// src/render/groundShapePool.ts:45-47 slice of src/render/groundShapePool.ts#GroundShapePool.digest (shared): the feature's statements; "…" marks code left out
  digest(ask: BlockDigestAsk): Promise<Float64Array[]> {
    return this.run({ kind: 'blocks', ask }) as Promise<Float64Array[]>;
  }

// src/render/groundShapePool.ts:49-51 slice of src/render/groundShapePool.ts#GroundShapePool.pads (shared): the feature's statements; "…" marks code left out
  pads(ask: PadsAsk): Promise<(Pad | null)[]> {
    return this.run({ kind: 'pads', ask }) as Promise<(Pad | null)[]>;
  }

// src/render/groundShapePool.ts:62-73 slice of src/render/groundShapePool.ts#GroundShapePool.run (shared): the feature's statements; "…" marks code left out
  private run(job: GroundJob): Promise<GroundJobAnswer> {
    // … 5 lines
    return new Promise((resolve, reject) => {
      // … 2 lines
      lane.worker.postMessage({ id, job });
    });
  }

// src/render/groundShapePool.ts:75-107 slice of src/render/groundShapePool.ts#GroundShapePool.start (shared): the feature's statements; "…" marks code left out
  private start(): Lane[] | null {
    // … 4 lines
    try {
      for (let k = 0; k < count; k++) {
        // … 2 lines
        worker.onmessage = (event: MessageEvent<{ id: number; answer?: GroundJobAnswer; error?: string }>) => {
          if (this.lanes !== lanes) return;
          lane.heard = performance.now();
          lane.told = false;
          const job = lane.waiting.get(event.data.id);
          if (!job) return;
          lane.waiting.delete(event.data.id);
          if (event.data.answer) job.resolve(event.data.answer);
          else job.reject(new Error(event.data.error ?? 'ground worker failed'));
        };
      // … 3 lines
      }
    // … 1 lines
    } catch (error) {
    // … 5 lines
    }
  // … 1 lines
  }
