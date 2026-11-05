// The code of src/render/groundShape.worker.ts that is the feature's (the rest of the file is shared).

// src/render/groundShape.worker.ts:5-21 slice of src/render/groundShape.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(self: JobScope): void {
  // … 2 lines
  const scratch = new ShapeScratch();
  // … 1 lines
  self.onmessage = (event: MessageEvent<{ readonly id: number; readonly job: GroundJob }>) => {
    const { id, job } = event.data;
    try {
      const { answer, transfer } = runGroundJob(job, scratch);
      (self as unknown as { postMessage(message: unknown, transfer: Transferable[]): void }).postMessage({ id, answer }, transfer);
    } catch (error) {
      // Reported here and answered: the page does this job itself (`groundShapePool.ts`).
      console.error('[ground] a job failed in its worker', error);
      self.postMessage({ id, error: String(error) });
    }
  };
}
