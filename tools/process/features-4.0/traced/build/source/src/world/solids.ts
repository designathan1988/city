// The code of src/world/solids.ts that is the feature's (the rest of the file is shared).

// src/world/solids.ts:18-18 slice of src/world/solids.ts#POST_ELEMENTS (shared): the feature's statements; "…" marks code left out
const POST_ELEMENTS: Readonly<Partial<Record<ElementKind, number>>> = { tree: m(0.25), lamp: m(0.12) };

// src/world/solids.ts:45-82 slice of src/world/solids.ts#solidsOfItem (shared): the feature's statements; "…" marks code left out
function solidsOfItem(doc: RoadDoc, key: SourceKey): Solid[] | undefined {
  // … 2 lines
  switch (key[0]) {
    case 'b': {
      const b = doc.buildings.get(id as BuildingId);
      for (const e of b?.elements ?? []) {
        const post = POST_ELEMENTS[e.kind];
        if (post === undefined || !onGround(e)) continue;
        const c = boxOf(elementRing(b!, e));
        out.push(disc({ x: (c[0] + c[2]) / 2, y: (c[1] + c[3]) / 2 }, post));
      }
    // … 1 lines
    }
  // … 21 lines
  }
// … 1 lines
}

// src/world/solids.ts:96-142 slice of src/world/solids.ts#createSolids (shared): the feature's statements; "…" marks code left out
export function createSolids(): { solids: Solids; derived: Derived<RoadDoc, SourceKey, Solid[]> } {
  // … 7 lines
  const derived = new Derived<RoadDoc, SourceKey, Solid[]>({
    // … 4 lines
    *allKeys(doc) {
      // … 1 lines
      for (const id of doc.buildings.idList()) yield `b${id}`;
    // … 3 lines
    },
  // … 10 lines
  });
// … 15 lines
}
