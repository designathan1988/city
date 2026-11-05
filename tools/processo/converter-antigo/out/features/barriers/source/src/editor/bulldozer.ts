// The code of src/editor/bulldozer.ts that is the feature's (the rest of the file is shared).

// src/editor/bulldozer.ts:125-149 slice of src/editor/bulldozer.ts#Bulldozer.boxed (shared): the feature's statements; "…" marks code left out
  private boxed(a: Vec2, b: Vec2): void {
    // … 11 lines
    const walls = doc.barriersIn(box).filter((bar) => bar.points.some(inside)).map((bar) => bar.id);
    if (!segments.length && !builtIds.length && !lots.length && !poles.length && !items.length && !walls.length) return;
    this.host.mutate(() => {
      // … 5 lines
      for (const id of walls) doc.removeBarrier(id);
    // … 1 lines
    });
  // … 1 lines
  }
