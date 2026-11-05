// The code of src/editor/chunkedSave.ts that is the feature's (the rest of the file is shared).

// src/editor/chunkedSave.ts:60-94 slice of src/editor/chunkedSave.ts#KEYED (shared): the feature's statements; "…" marks code left out
const KEYED: Readonly<Record<KeyedName, KeyedSpec>> = {
  // … 25 lines
  buildings: keyed<Building>({
    side: DENSE_SQUARE, kinds: ['buildings'], get: (doc, id) => doc.buildings.get(id as BuildingId), all: (doc) => doc.buildings.all(),
    at: (_doc, b) => b, saved: (b) => b,
  }),
  lots: keyed<Lot>({
    side: DENSE_SQUARE, kinds: ['lots'], get: (doc, id) => doc.lotStore.get(id), all: (doc) => doc.lotStore.all(), at: (_doc, lot) => lot.corners[0],
    saved: (lot) => lot, order: (doc, id) => doc.lotStore.orderOf(id) ?? 0,
  }),
};

// src/editor/chunkedSave.ts:476-564 slice of src/editor/chunkedSave.ts#TiledSaves.write (shared): the feature's statements; "…" marks code left out
  private async write(doc: RoadDoc, settings: unknown, savedAt: number): Promise<boolean> {
    // … 9 lines
    for (const name of KEYED_NAMES) {
      // … 6 lines
      for (const square of dirty) {
        // … 8 lines
        if (name === 'buildings') put.push({ key: this.wearKey(square), bytes: wearFrame(records as Building[]), packed: false });
      // … 1 lines
      }
      // … 1 lines
      for (const square of wear) {
        // … 2 lines
        if (records.length) put.push({ key: this.wearKey(square), bytes: wearFrame(records as Building[]), packed: false });
      }
    // … 1 lines
    }
  // … 51 lines
  }
