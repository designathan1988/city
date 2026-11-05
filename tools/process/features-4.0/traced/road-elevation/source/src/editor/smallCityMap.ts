// The code of src/editor/smallCityMap.ts that is the feature's (the rest of the file is shared).

// src/editor/smallCityMap.ts:97-113 slice of src/editor/smallCityMap.ts#growLots (shared): the feature's statements; "…" marks code left out
function growLots(doc: RoadDoc, net: Network, grew: (() => void) | null): void {
  // … 1 lines
  const ground = FLAT_GROUND;
  const elevation = buildRoadElevation(net, FLAT_GROUND);
  // … 3 lines
  for (let guard = 0; guard < open * 3 + 50; guard++) {
    const id = growOnLot({ doc, net, groundAt: ground, pavedAt }, refused, GROWTH_SEED);
  // … 6 lines
  }
}

// src/editor/smallCityMap.ts:101-101 slice of src/editor/smallCityMap.ts#growLots.pavedAt (shared): the feature's statements; "…" marks code left out
  const pavedAt = (x: number, y: number): number => (elevation.has(GROUND_ONLY) ? pavedHeightFrom(elevation, GROUND_ONLY, (px, py) => footwayRiseAt(net, px, py), x, y) : NaN);
