// What the feature uses of src/world/doc.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/doc.ts:153-2888 class RoadDoc
export class RoadDoc { … }

// src/world/doc.ts:164-164 getter RoadDoc.poles
get poles(): ReadonlyMap<PoleId, UtilityPole>;

// src/world/doc.ts:165-165 getter RoadDoc.poleSpans
get poleSpans(): ReadonlyMap<SpanId, UtilitySpan>;

// src/world/doc.ts:262-262 field RoadDoc.changes
/** What changed in this document, where and why (`changes.ts`) */
  readonly changes = new ChangeJournal();

// src/world/doc.ts:440-447 method RoadDoc.addPole
addPole(at: { x: number; y: number }, lamp = false): UtilityPole;

// src/world/doc.ts:450-452 method RoadDoc.polesIn
/** The poles standing in the cells a rectangle reaches (`PoleIndex.near`): a superset, test each. */
  polesIn(rect: ChangeRect): UtilityPole[];

// src/world/doc.ts:455-457 method RoadDoc.spansAt
/** The spans reaching a pole, in the order they were strung. */
  spansAt(id: PoleId): UtilitySpan[];

// src/world/doc.ts:498-509 method RoadDoc.addPoleSpan
/** Strings wire between two existing poles. Returns null for a degenerate run. */
  addPoleSpan(a: PoleId, b: PoleId): UtilitySpan | null;

// src/world/doc.ts:654-668 method RoadDoc.removePole
/** Removes a pole and every wire that reached it. */
  removePole(id: PoleId): void;

// src/world/doc.ts:671-684 method RoadDoc.poleNear
/** The pole nearest a point, within `radius`, or null. */
  poleNear(at: { x: number; y: number }, radius: number): UtilityPole | null;
