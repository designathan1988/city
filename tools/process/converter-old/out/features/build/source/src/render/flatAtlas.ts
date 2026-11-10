// The code of src/render/flatAtlas.ts that is the feature's (the rest of the file is shared).

// src/render/flatAtlas.ts:552-572 slice of src/render/flatAtlas.ts#createFlatAtlas.readContent (shared): the feature's statements; "…" marks code left out
  const readContent = (doc: TerrainSource): boolean => {
    // … 1 lines
    const key = [d.revision, d.buildings?.revision, d.lotRevision, d.utilityRevision, d.treeRevision, doc.terrainRevision, doc.paintRevision].join(':');
    if (key === contentKey) return false;
    contentKey = key;
  // … 15 lines
  };

// src/render/flatAtlas.ts:584-602 slice of src/render/flatAtlas.ts#createFlatAtlas.readWholeContent (shared): the feature's statements; "…" marks code left out
  const readWholeContent = (doc: TerrainSource): boolean => {
    // … 5 lines
    for (const b of d.buildings?.all() ?? []) mark(b.x, b.y);
    for (const l of d.lots ?? []) for (const c of l.corners) mark(c.x, c.y);
  // … 10 lines
  };
