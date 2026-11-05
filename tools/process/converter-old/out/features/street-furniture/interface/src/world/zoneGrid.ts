// What the feature uses of src/world/zoneGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/zoneGrid.ts:108-112 function markPaved
/** DO NOT go back to testing every cell against the network. */

/** Every `NEAR` cell of `label` whose centre is paved turned to `PAVED`, the other `NEAR` cells to `LAND` */
export function* markPaved(doc: RoadDoc, net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void>;
