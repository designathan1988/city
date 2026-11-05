// The code of src/editor/transitTools.ts that is the feature's (the rest of the file is shared).

// src/editor/transitTools.ts:54-61 slice of src/editor/transitTools.ts#TransitTool.built (shared): the feature's statements; "…" marks code left out
  private built(p: Vec2): boolean {
    // … 2 lines
    for (const b of this.deps.doc().buildings.near(p.x - pad, p.y - pad, p.x + pad, p.y + pad)) {
      for (const v of b.volumes) if (!v.open && pointInPolygon(p, volumeCorners(b, v, pad))) return true;
    }
  // … 1 lines
  }
