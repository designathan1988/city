// The code of src/editor/transitTools.ts that is the feature's (the rest of the file is shared).

// src/editor/transitTools.ts:18-25 slice of src/editor/transitTools.ts#TransitToolDeps (shared): the feature's statements; "…" marks code left out
export interface TransitToolDeps {
  // … 1 lines
  readonly net: () => Network;
// … 4 lines
}

// src/editor/transitTools.ts:69-69 slice of src/editor/transitTools.ts#TransitTool.onRoad (shared): the feature's statements; "…" marks code left out
  onRoad(p: Vec2): boolean { return onCarriageway(this.deps.net(), p); }
