// The code of src/world/parkingLayout.ts that is the feature's (the rest of the file is shared).

// src/world/parkingLayout.ts:212-218 slice of src/world/parkingLayout.ts#entrancesNear (shared): the feature's statements; "…" marks code left out
function entrancesNear(doc: RoadDoc, line: Polyline, half: number): [Vec2, Vec2][] {
  // … 3 lines
  for (const b of doc.buildings.near(bb.minX - reach, bb.minY - reach, bb.maxX + reach, bb.maxY + reach)) out.push(...entrancesOf(b));
// … 1 lines
}
