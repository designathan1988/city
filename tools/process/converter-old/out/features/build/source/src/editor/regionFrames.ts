// The code of src/editor/regionFrames.ts that is the feature's (the rest of the file is shared).

// src/editor/regionFrames.ts:16-21 slice of src/editor/regionFrames.ts#squareFrame (shared): the feature's statements; "…" marks code left out
export function squareFrame(buildings: boolean, records: readonly unknown[], orders: readonly number[] | null | undefined): ArrayBuffer {
  const body = buildings
    ? { kind: K_TABLE, bytes: new Uint8Array(encodeRecords(records as Building[]).buffer) }
    : { kind: K_JSON, bytes: jsonBytes(records) };
// … 1 lines
}

// src/editor/regionFrames.ts:24-33 slice of src/editor/regionFrames.ts#readSquareFrame (shared): the feature's statements; "…" marks code left out
export function readSquareFrame(bytes: Uint8Array): SquareRecords | null {
  // … 4 lines
  const r: unknown = table ? tableRecords(table.slice().buffer) : json ? jsonOf(json) : null;
  // … 1 lines
  if (!Array.isArray(r) || (orders && (!o || o.length !== r.length))) return null;
  return o ? { r, o: Array.from(o) } : { r };
}

// src/editor/regionFrames.ts:36-40 slice of src/editor/regionFrames.ts#wearFrame (shared): the feature's statements; "…" marks code left out
export function wearFrame(records: readonly Building[]): ArrayBuffer {
  // … 1 lines
  for (const b of records) pairs.push(b.id, b.decay ?? 0);
// … 1 lines
}
