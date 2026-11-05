// The code of src/sim/agents/kerbBays.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/kerbBays.ts:58-69 function kerbStreetsIn: the feature's own
export function kerbStreetsIn(book: KerbBook, x0: number, y0: number, x1: number, y1: number): SegmentId[] {
  const out = new Set<SegmentId>();
  for (let i = Math.floor(x0 / KERB_CELL); i <= Math.floor(x1 / KERB_CELL); i++) {
    for (let j = Math.floor(y0 / KERB_CELL); j <= Math.floor(y1 / KERB_CELL); j++) {
      for (const id of book.streets.get(kerbCell(i, j)) ?? []) {
        const r = book.reach.get(id);
        if (r && r.x0 <= x1 && r.x1 >= x0 && r.y0 <= y1 && r.y1 >= y0) out.add(id);
      }
    }
  }
  return [...out].sort((a, b) => a - b);
}
