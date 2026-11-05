// What the feature uses of src/editor/growthFeed.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/growthFeed.ts:36-39 interface GrowthLane
/** Where the growth's worker answers (`growthPool.ts`; a spec's stand-in). */
export interface GrowthLane {
  /** `transfer`: buffers made for this ask alone, handed over rather than copied (MDN, "Transferable objects"). */
  post(ask: GrowAsk, transfer: readonly ArrayBuffer[]): void;
}

// src/editor/growthFeed.ts:72-333 class GrowthSession
export class GrowthSession { … }

// src/editor/growthFeed.ts:227-274 method GrowthSession.send
/** The ask for `lot` (picked here, `pickLot`, its stream `rng`) and the lots the growth may go on to in the worker: the open lots round it, with what they all read - the buildings, roads and ground round them, the paving there - sent once and kept in step (`syncBuildings`, `syncRoads`), the ground and the paving of the region read for this ask. */
  private send(lot: Lot, rng: RngState, refused: Set<number>, extra: number, misses: number): void;
