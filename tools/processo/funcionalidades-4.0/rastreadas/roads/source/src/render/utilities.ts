// The code of src/render/utilities.ts that is the feature's (the rest of the file is shared).

// src/render/utilities.ts:524-537 slice of src/render/utilities.ts#poleGroundAt (shared): the feature's statements; "…" marks code left out
export function poleGroundAt(
  elevation: RoadElevation,
  // … 2 lines
  net?: Network,
): (x: number, y: number) => number {
  return (x, y) => {
    const road = elevation.roadAt(x, y, GROUND_ONLY);
    // … 1 lines
    const onFootway = road.type >= 0 && Math.abs(road.across) <= road.half;
    if (!onFootway) return terrainAt(x, y);
    return elevation.at(x, y, GROUND_ONLY) + (net ? footwayRiseAt(net, x, y) : FOOTWAY_RISE);
  };
}
