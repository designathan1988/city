// The code of src/world/solids.ts that is the feature's (the rest of the file is shared).

// src/world/solids.ts:45-82 slice of src/world/solids.ts#solidsOfItem (shared): the feature's statements; "…" marks code left out
function solidsOfItem(doc: RoadDoc, key: SourceKey): Solid[] | undefined {
  // … 2 lines
  switch (key[0]) {
    // … 10 lines
    case 'r': {
      const bar = doc.barriers.get(id);
      if (!bar) break;
      const r = m(BARRIER_SIZE[bar.kind].thickness) / 2;
      for (let i = 1; i < bar.points.length; i++) {
        const a = bar.points[i - 1]!, b = bar.points[i]!;
        out.push({ kind: 'capsule', a, b, r, box: boxOf([a, b], r) });
      }
    // … 1 lines
    }
    case 'p': {
      const p = doc.poles.get(id as PoleId);
      if (p) out.push(disc({ x: p.x, y: p.y }, POLE));
    // … 1 lines
    }
  // … 6 lines
  }
// … 1 lines
}

// src/world/solids.ts:96-142 slice of src/world/solids.ts#createSolids (shared): the feature's statements; "…" marks code left out
export function createSolids(): { solids: Solids; derived: Derived<RoadDoc, SourceKey, Solid[]> } {
  // … 7 lines
  const derived = new Derived<RoadDoc, SourceKey, Solid[]>({
    // … 4 lines
    *allKeys(doc) {
      // … 2 lines
      for (const id of doc.barriers.keys()) yield `r${id}`;
      for (const id of doc.poles.keys()) yield `p${id}`;
    // … 1 lines
    },
  // … 10 lines
  });
// … 15 lines
}
