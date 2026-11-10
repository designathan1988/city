// The code of src/world/zoneGrid.ts that is the feature's (the rest of the file is shared).

// src/world/zoneGrid.ts:101-101 slice of src/world/zoneGrid.ts#pavedTester.onPlate (shared): the feature's statements; "…" marks code left out
  const onPlate = (p: Vec2): boolean => onPaving(net, p);

// src/world/zoneGrid.ts:108-112 slice of src/world/zoneGrid.ts#markPaved (shared): the feature's statements; "…" marks code left out
export function* markPaved(doc: RoadDoc, net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void> {
  // … 1 lines
  yield* markPlates(net, grid, label);
// … 1 lines
}
