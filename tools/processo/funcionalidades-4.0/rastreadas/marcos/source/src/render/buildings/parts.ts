// The code of src/render/buildings/parts.ts that is the feature's (the rest of the file is shared).

// src/render/buildings/parts.ts:64-120 slice of src/render/buildings/parts.ts#partSample (shared): the feature's statements; "…" marks code left out
function partSample(id: string): Building | null {
  // … 1 lines
  if (id.startsWith('lot:')) {
    const lot = loadedLot(id.slice(4));
    return lot ? ({ ...instantiate(lot.body as BlueprintBody, { x: 0, y: 0 }, Math.PI / 4), id: -1 } as Building) : null;
  }
// … 50 lines
}

// src/render/buildings/parts.ts:281-303 slice of src/render/buildings/parts.ts#createThumbnailStudio.request (shared): the feature's statements; "…" marks code left out
    request(ids, onBatch) {
      // … 8 lines
      const lots = fresh.filter((id) => id.startsWith('lot:')).map((id) => loadLot(id.slice(4)));
      void Promise.all(lots).then(() => keptPictures()).then((pictures) => {
        const ready = new Map<string, string>();
        for (const id of fresh) {
          const blob = pictures.get(id);
          if (blob) {
            ready.set(id, URL.createObjectURL(blob));
            done.add(id);
          } else waiting.push({ id, onBatch });
        }
        if (ready.size) onBatch(ready);
        if (frame === null && waiting.length > 0) frame = requestAnimationFrame(step);
      });
    },
