// The code of src/editor/chunkedSave.ts that is the feature's (the rest of the file is shared).

// src/editor/chunkedSave.ts:60-94 slice of src/editor/chunkedSave.ts#KEYED (shared): the feature's statements; "…" marks code left out
const KEYED: Readonly<Record<KeyedName, KeyedSpec>> = {
  // … 20 lines
  barriers: keyed<Barrier>({
    side: SAVE_TILE, kinds: ['barriers'], get: (doc, id) => doc.barriers.get(id), all: (doc) => doc.barriers.values(), at: (_doc, b) => b.points[0],
    saved: savedBarrier,
  }),
// … 9 lines
};
