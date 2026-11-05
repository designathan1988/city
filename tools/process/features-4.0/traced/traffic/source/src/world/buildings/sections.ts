// The code of src/world/buildings/sections.ts that is the feature's (the rest of the file is shared).

// src/world/buildings/sections.ts:42-45 function sectionsSize: the feature's own
export function sectionsSize(s: BuildingSections): number {
  const rows = (buffer: ArrayBufferLike | null): number => (buffer ? new RecordTable(buffer).rows : 0);
  return rows(s.base) - s.gone.length + rows(s.added);
}

// src/world/buildings/sections.ts:74-94 function eachSectionRef: the feature's own
export function* eachSectionRef(s: BuildingSections): Generator<RecordRef, void, void> {
  const replaced = new Map<number, number>();
  if (s.replaced) {
    const table = new RecordTable(s.replaced);
    for (let row = 0; row < table.rows; row++) replaced.set(table.ids[row]!, row);
  }
  const gone = new Set<number>(s.gone);
  if (s.base) {
    const table = new RecordTable(s.base);
    for (let row = 0; row < table.rows; row++) {
      const id = table.ids[row]!;
      if (gone.has(id)) continue;
      const over = replaced.get(id);
      yield over === undefined ? { buffer: s.base, row } : { buffer: s.replaced!, row: over };
    }
  }
  if (s.added) {
    const rows = new RecordTable(s.added).rows;
    for (let row = 0; row < rows; row++) yield { buffer: s.added, row };
  }
}
