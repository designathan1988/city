// The code of src/render/flatAtlas.ts that is the feature's (the rest of the file is shared).

// src/render/flatAtlas.ts:584-602 slice of src/render/flatAtlas.ts#createFlatAtlas.readWholeContent (shared): the feature's statements; "…" marks code left out
  const readWholeContent = (doc: TerrainSource): boolean => {
    // … 7 lines
    for (const p of d.poles?.values() ?? []) mark(p.x, p.y);
    // … 2 lines
    for (const barrier of d.barriers?.values() ?? []) for (const q of barrier.points) mark(q.x, q.y);
  // … 6 lines
  };
