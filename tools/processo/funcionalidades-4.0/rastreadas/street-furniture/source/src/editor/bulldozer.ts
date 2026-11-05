// The code of src/editor/bulldozer.ts that is the feature's (the rest of the file is shared).

// src/editor/bulldozer.ts:94-122 slice of src/editor/bulldozer.ts#Bulldozer.click (shared): the feature's statements; "…" marks code left out
  private click(screen: Vec2, world: Vec2, anchor: Anchor): void {
    // … 8 lines
    if (pole) {
      host.mutate(() => {
        doc.removePole(pole.id);
      // … 1 lines
      });
    // … 2 lines
    }
  // … 11 lines
  }

// src/editor/bulldozer.ts:125-149 slice of src/editor/bulldozer.ts#Bulldozer.boxed (shared): the feature's statements; "…" marks code left out
  private boxed(a: Vec2, b: Vec2): void {
    // … 9 lines
    const poles = doc.polesIn([x0, y0, x1, y1]).filter((p) => inside(p)).sort((p, q) => p.id - q.id).map((p) => p.id);
    // … 1 lines
    const walls = doc.barriersIn(box).filter((bar) => bar.points.some(inside)).map((bar) => bar.id);
    if (!segments.length && !builtIds.length && !lots.length && !poles.length && !items.length && !walls.length) return;
    this.host.mutate(() => {
      // … 3 lines
      for (const id of poles) doc.removePole(id);
      // … 1 lines
      for (const id of walls) doc.removeBarrier(id);
    // … 1 lines
    });
  // … 1 lines
  }
