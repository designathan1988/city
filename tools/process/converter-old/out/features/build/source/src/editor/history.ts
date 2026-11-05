// The code of src/editor/history.ts that is the feature's (the rest of the file is shared).

// src/editor/history.ts:171-174 slice of src/editor/history.ts#openedGenerated (shared): the feature's statements; "…" marks code left out
export function openedGenerated(data: SerializedDoc, table: ArrayBufferLike | null): boolean {
  const buildings = (data.buildings?.length ?? 0) + (table ? new RecordTable(table).rows : 0);
  return buildings > 10_000 && (data.terrain?.length ?? 0) === 0 && data.relief === RELIEF_FLAT;
}
