// The code of src/sim/agents/parking.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/parking.ts:344-351 function baySourcesIn: the feature's own
export function baySourcesIn(w: BayWorld, x0: number, y0: number, x1: number, y1: number): string[] {
  const book = baysNow(w);
  const out: string[] = [];
  // `near` answers in the store's order, the lots' own.
  for (const id of w.buildings.nearIds(x0, y0, x1, y1)) if (book.known.has(id)) out.push(`L${id}`);
  for (const id of kerbStreetsIn(book.kerb, x0, y0, x1, y1)) if (book.kerb.bays.get(id)?.length) out.push(`K${id}`);
  return out;
}
