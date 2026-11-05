// The code of src/world/landscape.ts that is the feature's (the rest of the file is shared).

// src/world/landscape.ts:94-124 slice of src/world/landscape.ts#footwayAt (shared): the feature's statements; "…" marks code left out
export function footwayAt(net: Network, at: Vec2, reach = 0): FootwayHit | null {
  // … 3 lines
  for (const ribbon of ribbonsNear(net, at, reach)) {
    // … 1 lines
    if (!carriesPedestrians(road) || road.sidewalk <= 0) continue;
  // … 21 lines
  }
// … 1 lines
}
