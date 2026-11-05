// What the feature uses of src/editor/growthFeed.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/growthFeed.ts:36-39 interface GrowthLane
/** Where the growth's worker answers (`growthPool.ts`; a spec's stand-in). */
export interface GrowthLane {
  /** `transfer`: buffers made for this ask alone, handed over rather than copied (MDN, "Transferable objects"). */
  post(ask: GrowAsk, transfer: readonly ArrayBuffer[]): void;
}

// src/editor/growthFeed.ts:72-333 class GrowthSession
export class GrowthSession { … }
