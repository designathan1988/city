// The code of src/editor/transitTools.ts that is the feature's (the rest of the file is shared).

// src/editor/transitTools.ts:64-66 slice of src/editor/transitTools.ts#TransitTool.footwayAt (shared): the feature's statements; "…" marks code left out
  private footwayAt(p: Vec2, reach = STOP_REACH, clear = false): { x: number; y: number; segment: number } | null {
    return nearestFootway(this.deps.net(), p, reach, clear ? (q) => this.built(q) : undefined);
  }
