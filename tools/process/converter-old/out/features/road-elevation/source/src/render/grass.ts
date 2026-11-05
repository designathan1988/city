// The code of src/render/grass.ts that is the feature's (the rest of the file is shared).

// src/render/grass.ts:41-97 slice of src/render/grass.ts#grassClump (shared): the feature's statements; "…" marks code left out
export function grassClump(
  elevation: RoadElevation,
// … 2 lines
): Clump {
  // … 3 lines
  const road = elevation.roadAt(cx, cy);
  if (road.type >= 0 && Math.abs(road.across) < road.half + radius * 0.6 + m(0.32)) return NONE;
  // … 12 lines
  for (let i = 0; i < count; i++) {
    // … 6 lines
    if (road.type >= 0 && Math.abs(road.across) - r < road.half + m(0.24)) {
      const near = elevation.roadAt(x, y);
      if (near.type >= 0 && Math.abs(near.across) < near.half + m(0.24)) continue;
    }
  // … 21 lines
  }
// … 1 lines
}
