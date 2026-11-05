// The code of src/world/rescale.ts that is the feature's (the rest of the file is shared).

// src/world/rescale.ts:78-132 slice of src/world/rescale.ts#rescaleSerializedDoc (shared): the feature's statements; "…" marks code left out
export function rescaleSerializedDoc(data: SerializedDoc): SerializedDoc {
  // … 33 lines
  out.zones = list(data.zones, (z) => {
    if (!isRecord(z) || ![z.x0, z.y0, z.x1, z.y1].every(finite)) return z;
    const a = place(z.x0 as number, z.y0 as number), b = place(z.x1 as number, z.y1 as number);
    return { ...z, x0: Math.min(a.x, b.x), y0: Math.min(a.y, b.y), x1: Math.max(a.x, b.x), y1: Math.max(a.y, b.y) };
  });
  // … 1 lines
  out.lots = list(data.lots, (lot) => (isRecord(lot) && Array.isArray(lot.corners) ? { ...lot, corners: lot.corners.map(at) } : lot));
// … 13 lines
}
