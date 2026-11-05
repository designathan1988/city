// What the feature uses of src/world/zoneGrid.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/zoneGrid.ts:87-103 function pavedTester
/** What land may not be built on: any ground road's carriageway and footway */
export function pavedTester(doc: RoadDoc, net: Network): { onRoad: (p: Vec2) => boolean; onPlate: (p: Vec2) => boolean };

// src/world/zoneGrid.ts:108-112 function markPaved
/** DO NOT go back to testing every cell against the network. */

/** Every `NEAR` cell of `label` whose centre is paved turned to `PAVED`, the other `NEAR` cells to `LAND` */
export function* markPaved(doc: RoadDoc, net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void>;

// src/world/zoneGrid.ts:115-115 variable SWEEP_SLACK
/** How much wider than a street's reach its capsules are scanned: far more than any rounding. */
const SWEEP_SLACK = m(0.5);

// src/world/zoneGrid.ts:118-175 function markRoads
/** `onRoad` (`zoneGrid.ts`) on the raster */
function* markRoads(doc: RoadDoc, net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void>;
