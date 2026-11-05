// What the feature uses of src/editor/transitTools.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/transitTools.ts:18-25 interface TransitToolDeps
export interface TransitToolDeps {
  readonly doc: () => RoadDoc;
  readonly net: () => Network;
  /** The transport replaced, as one undo step. */
  readonly edit: (next: TransitData) => void;
  readonly hint: (key: string) => void;
  readonly redraw: () => void;
}

// src/editor/transitTools.ts:32-244 class TransitTool
export class TransitTool { … }

// src/editor/transitTools.ts:69-69 method TransitTool.onRoad
/** Whether a point is on a road's carriageway. */
  onRoad(p: Vec2): boolean;
