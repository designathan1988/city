// The code of src/world/solids.ts that is the feature's (the rest of the file is shared).

// src/world/solids.ts:20-23 slice of src/world/solids.ts#FURNITURE (shared): the feature's statements; "…" marks code left out
const FURNITURE: Readonly<Partial<Record<LandscapeKind, number>>> = {
  tree: m(0.3), shrub: m(0.45), bench: m(0.55), bin: m(0.3), lamp: m(0.15), hydrant: m(0.2),
  postbox: m(0.3), phone: m(0.5), sign: m(0.08), streetname: m(0.08),
};

// src/world/solids.ts:45-82 slice of src/world/solids.ts#solidsOfItem (shared): the feature's statements; "…" marks code left out
function solidsOfItem(doc: RoadDoc, key: SourceKey): Solid[] | undefined {
  // … 2 lines
  switch (key[0]) {
    // … 25 lines
    case 'l': {
      const item = doc.landscape.get(id);
      const r = item ? FURNITURE[item.kind] : undefined;
      if (item && r !== undefined) out.push(disc({ x: item.x, y: item.y }, r));
    // … 1 lines
    }
  }
// … 1 lines
}

// src/world/solids.ts:96-142 slice of src/world/solids.ts#createSolids (shared): the feature's statements; "…" marks code left out
export function createSolids(): { solids: Solids; derived: Derived<RoadDoc, SourceKey, Solid[]> } {
  // … 7 lines
  const derived = new Derived<RoadDoc, SourceKey, Solid[]>({
    // … 4 lines
    *allKeys(doc) {
      // … 4 lines
      for (const id of doc.landscape.keys()) yield `l${id}`;
    },
  // … 10 lines
  });
// … 15 lines
}
