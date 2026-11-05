// The code of src/bays.worker.ts that is the feature's (the rest of the file is shared).

// src/bays.worker.ts:5-24 slice of src/bays.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(self: JobScope): void {
  // … 3 lines
  self.onmessage = (event: MessageEvent<{ id: number; share: BayShare }>) => {
    const { id, share } = event.data;
    let spoke = performance.now();
    const beat = (): void => {
      const now = performance.now();
      if (now - spoke >= BEAT_MS) { spoke = now; self.postMessage({ id, beat: true }); }
    };
    try {
      self.postMessage({ id, bays: baysOfShare(share, beat) });
    } catch (error) {
      // Reported here and answered: the page works the lots out itself (`parking.ts` `baysNow`).
      console.error('[parking] a share of lots failed in its worker', error);
      self.postMessage({ id, error: String(error) });
    }
  };
}
