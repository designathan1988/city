// The code of src/editor/chunkedSave.ts that is the feature's (the rest of the file is shared).

// src/editor/chunkedSave.ts:60-94 slice of src/editor/chunkedSave.ts#KEYED (shared): the feature's statements; "…" marks code left out
const KEYED: Readonly<Record<KeyedName, KeyedSpec>> = {
  // … 8 lines
  poles: keyed<UtilityPole>({
    side: SAVE_TILE, kinds: ['utilities'], get: (doc, id) => doc.poles.get(id as PoleId), all: (doc) => doc.poles.values(), at: (_doc, p) => p,
    saved: savedPole, reaches: (doc, p) => doc.spansAt(p.id).map((s) => ['spans', s.id] as const),
  }),
  spans: keyed<UtilitySpan>({
    side: SAVE_TILE, kinds: ['utilities'], get: (doc, id) => doc.poleSpans.get(id as SpanId), all: (doc) => doc.poleSpans.values(),
    at: (doc, s) => doc.poles.get(s.a), saved: savedSpan,
  }),
  // … 4 lines
  barriers: keyed<Barrier>({
    side: SAVE_TILE, kinds: ['barriers'], get: (doc, id) => doc.barriers.get(id), all: (doc) => doc.barriers.values(), at: (_doc, b) => b.points[0],
    saved: savedBarrier,
  }),
// … 9 lines
};
