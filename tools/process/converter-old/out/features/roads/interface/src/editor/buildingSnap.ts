// What the feature uses of src/editor/buildingSnap.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/buildingSnap.ts:14-18 interface PlacementSnap
/** Where a footprint goes when the pointer is at `cursor`: to a road first */
export interface PlacementSnap {
  readonly anchor: Vec2;
  readonly rotation: number;
  readonly kind: 'road' | 'building' | 'grid';
}

// src/editor/buildingSnap.ts:21-25 interface FootprintSize
/** The size of a footprint, measured from its anchor. */
export interface FootprintSize {
  readonly width: number;
  readonly depth: number;
  readonly module: number;
}

// src/editor/buildingSnap.ts:33-33 variable ROAD_REACH
/** How far from a road a footprint still snaps to it, beyond its own depth. */
const ROAD_REACH = m(9.6);

// src/editor/buildingSnap.ts:40-64 function snapPlacement
/** @perInput Runs once a pointer move (the snap under the cursor): nothing it reaches may walk the map. */
export function snapPlacement(
  doc: RoadDoc,
  net: Network | null,
  size: FootprintSize,
  cursor: Vec2,
  rotation: number,
  ignore?: BuildingId,
): PlacementSnap;

// src/editor/buildingSnap.ts:75-106 function snapToRoad
/** DO NOT walk `net.ribbons` here: this runs on every pointer move. */
function snapToRoad(net: Network, size: FootprintSize, cursor: Vec2): PlacementSnap | null;

// src/editor/buildingSnap.ts:109-145 function cornerFlush
/** At a corner, the side of the footprint next to the crossing road slid onto the back of that road's footway */
function cornerFlush(net: Network, anchor: Vec2, rotation: number, size: FootprintSize, snapped: number, cursor: Vec2): Vec2;

// src/editor/buildingSnap.ts:168-225 function flush
/** Makes the footprint's edges flush with a neighbour of the same bearing: along the local x axis always */
function flush(doc: RoadDoc, snap: PlacementSnap, size: FootprintSize, both: boolean, ignore?: BuildingId): PlacementSnap;
