// The code of src/editor/bulldozer.ts that is the feature's (the rest of the file is shared).

// src/editor/bulldozer.ts:94-122 slice of src/editor/bulldozer.ts#Bulldozer.click (shared): the feature's statements; "…" marks code left out
  private click(screen: Vec2, world: Vec2, anchor: Anchor): void {
    // … 17 lines
    const item = onRoad ? null : landscapeNearIn(doc, world, host.itemReach());
    if (item) {
      host.mutate(() => doc.removeLandscape(item.id));
      host.hint('hint.streetscape.removed');
      return;
    }
  // … 4 lines
  }

// src/editor/bulldozer.ts:125-149 slice of src/editor/bulldozer.ts#Bulldozer.boxed (shared): the feature's statements; "…" marks code left out
  private boxed(a: Vec2, b: Vec2): void {
    // … 10 lines
    const items = doc.landscapeIn(box).filter((it) => inside(it)).map((it) => it.id);
    // … 1 lines
    if (!segments.length && !builtIds.length && !lots.length && !poles.length && !items.length && !walls.length) return;
    this.host.mutate(() => {
      // … 4 lines
      for (const id of items) doc.removeLandscape(id);
    // … 2 lines
    });
  // … 1 lines
  }
