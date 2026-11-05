// The code of src/editor/chunkedSave.ts that is the feature's (the rest of the file is shared).

// src/editor/chunkedSave.ts:60-94 slice of src/editor/chunkedSave.ts#KEYED (shared): the feature's statements; "…" marks code left out
const KEYED: Readonly<Record<KeyedName, KeyedSpec>> = {
  // … 16 lines
  landscape: keyed<LandscapeItem>({
    side: SAVE_TILE, kinds: ['landscape'], get: (doc, id) => doc.landscape.get(id), all: (doc) => doc.landscape.values(), at: (_doc, item) => item,
    saved: savedLandscape,
  }),
// … 13 lines
};
