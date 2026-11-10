// The code of src/render/flatAtlas.ts that is the feature's (the rest of the file is shared).

// src/render/flatAtlas.ts:390-397 slice of src/render/flatAtlas.ts#createFlatAtlas.takeStamps (shared): the feature's statements; "…" marks code left out
  const takeStamps = (p: Plate, doc: TerrainSource): boolean => {
    // … 1 lines
    if (p.source.terrainRevision !== 0 && sameList(sources, p.source.stampSources)) return false;
  // … 4 lines
  };

// src/render/flatAtlas.ts:399-406 slice of src/render/flatAtlas.ts#createFlatAtlas.takePaint (shared): the feature's statements; "…" marks code left out
  const takePaint = (p: Plate, doc: TerrainSource, edited: readonly ChangeRect[] | null): void => {
    // … 1 lines
    const next = plateDabs(p.source.paintSources, p.source.laid, reaching, edited);
    if (!next) return;
    p.source.paintSources = next;
    p.source.terrainPaint = next.map((d) => p.dabCopies.get(d) as PaintDab);
  // … 1 lines
  };
