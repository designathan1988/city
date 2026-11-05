// The code of src/world/cityGen/squares.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/squares.ts:65-91 slice of src/world/cityGen/squares.ts#mainRoadsNear (shared): the feature's statements; "…" marks code left out
function mainRoadsNear(doc: RoadDoc): (p: Vec2, reach: number) => boolean {
  const main = new Set(['avenue', 'urban', 'boulevard'].map((id) => ROAD_TYPES.findIndex((t) => t.id === id)));
  // … 1 lines
  for (const s of doc.segments.values()) {
    if (!main.has(s.type)) continue;
  // … 8 lines
  }
// … 12 lines
}
