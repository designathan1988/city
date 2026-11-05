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

// src/editor/transitTools.ts:30-30 variable STOP_REACH
/** How near a street a bus stop can be put. */
const STOP_REACH = m(18);

// src/editor/transitTools.ts:32-244 class TransitTool
export class TransitTool { … }

// src/editor/transitTools.ts:64-66 method TransitTool.footwayAt
/** DO NOT build the walking network of the whole map for a click */
  private footwayAt(p: Vec2, reach = STOP_REACH, clear = false): { x: number; y: number; segment: number } | null;

// src/editor/transitTools.ts:69-69 method TransitTool.onRoad
/** Whether a point is on a road's carriageway. */
  onRoad(p: Vec2): boolean;
