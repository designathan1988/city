// The code of src/editor/bulldozer.ts that is the feature's (the rest of the file is shared).

// src/editor/bulldozer.ts:125-149 slice of src/editor/bulldozer.ts#Bulldozer.boxed (shared): the feature's statements; "…" marks code left out
  private boxed(a: Vec2, b: Vec2): void {
    // … 6 lines
    const builtIds = doc.buildings.near(x0, y0, x1, y1).filter((bd) => inside({ x: bd.x, y: bd.y })).map((bd) => bd.id);
    // … 1 lines
    const lots = doc.lotsIn(box).filter((l) => inside(lotCentre(l))).map((l) => l.id);
    // … 3 lines
    if (!segments.length && !builtIds.length && !lots.length && !poles.length && !items.length && !walls.length) return;
    this.host.mutate(() => {
      // … 1 lines
      for (const id of lots) deleteLot(doc, id);
      for (const id of builtIds) doc.buildings.remove(id);
    // … 4 lines
    });
  // … 1 lines
  }
