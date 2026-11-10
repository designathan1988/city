// The code of src/render/utilities.ts that is the feature's (the rest of the file is shared).

// src/render/utilities.ts:524-537 slice of src/render/utilities.ts#poleGroundAt (shared): the feature's statements; "…" marks code left out
export function poleGroundAt(
// … 4 lines
): (x: number, y: number) => number {
  return (x, y) => {
    // … 4 lines
    return elevation.at(x, y, GROUND_ONLY) + (net ? footwayRiseAt(net, x, y) : FOOTWAY_RISE);
  };
}
