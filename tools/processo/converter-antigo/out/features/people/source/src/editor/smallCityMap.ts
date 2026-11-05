// The code of src/editor/smallCityMap.ts that is the feature's (the rest of the file is shared).

// src/editor/smallCityMap.ts:101-101 slice of src/editor/smallCityMap.ts#growLots.pavedAt (shared): the feature's statements; "…" marks code left out
  const pavedAt = (x: number, y: number): number => (elevation.has(GROUND_ONLY) ? pavedHeightFrom(elevation, GROUND_ONLY, (px, py) => footwayRiseAt(net, px, py), x, y) : NaN);
