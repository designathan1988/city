// The code of src/render/networkFarRoads.worker.ts that is the feature's (the rest of the file is shared).

// src/render/networkFarRoads.worker.ts:5-18 slice of src/render/networkFarRoads.worker.ts#serve (shared): the feature's statements; "…" marks code left out
export function serve(self: JobScope): void {
  self.onmessage = (event: MessageEvent<{ readonly id: number; readonly input: FarRoadInput }>) => {
    const { id, input } = event.data;
    try {
      const cells = buildFarRoadCells(input);
      const transfer: ArrayBuffer[] = [];
      for (const c of cells) transfer.push(c.positions.buffer as ArrayBuffer, c.colors.buffer as ArrayBuffer, c.indices.buffer as ArrayBuffer);
      self.postMessage({ id, cells }, transfer);
    } catch (error) {
      console.error('[roads] the far roads could not be made', error);
      self.postMessage({ id, error: String((error as Error)?.stack ?? error) });
    }
  };
}
