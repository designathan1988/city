// The code of src/editor/save.worker.ts that is the feature's (the rest of the file is shared).

// src/editor/save.worker.ts:9-9 slice of src/editor/save.worker.ts#SaveMessage (shared): the feature's statements; "…" marks code left out
type SaveMessage = { readonly id: number; readonly batch: SaveBatch };

// src/editor/save.worker.ts:12-23 slice of src/editor/save.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(self: JobScope): void {
  // … 3 lines
  self.onmessage = (event: MessageEvent<SaveMessage>) => {
    // … 1 lines
    void turns.write(() => writeBatch(store, batch)).then(
      (ok) => self.postMessage({ id, ok }),
      (error: unknown) => { console.error('[save] the worker could not write the save', error); self.postMessage({ id, ok: false }); },
    );
  };
}
