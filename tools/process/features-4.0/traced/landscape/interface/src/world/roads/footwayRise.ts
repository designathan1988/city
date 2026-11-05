// What the feature uses of src/world/roads/footwayRise.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/roads/footwayRise.ts:12-12 type FlushRoad
/** What `flushHolds` reads of a road: its widths and which footway is flush. */
type FlushRoad = Pick<RoadType, 'width' | 'sidewalk' | 'sidewalkLeft' | 'sidewalkRight' | 'flushLeft' | 'flushRight'>;

// src/world/roads/footwayRise.ts:33-33 function footwayRiseAt
/** The footway's height over the carriageway at (x, y), world units. */
export const footwayRiseAt = (net: Network, x: number, y: number): number => …;

// src/world/roads/footwayRise.ts:36-41 interface FlushData
/** The roads with a footway laid flush whose filed box (`Network.flushCells`) meets some rectangles */
export interface FlushData {
  readonly rects: Float64Array;
  readonly lineEnds: Uint32Array;
  readonly lines: Float64Array;
  readonly roads: readonly FlushRoad[];
}
