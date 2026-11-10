// The code of src/world/roads/buildMode.ts that is the feature's (the rest of the file is shared).

// src/world/roads/buildMode.ts:82-107 slice of src/world/roads/buildMode.ts#stationsAlong (shared): the feature's statements; "…" marks code left out
export function stationsAlong(
  // … 1 lines
  elevation: RoadElevation,
// … 3 lines
): RoadStation[] {
  // … 2 lines
  for (const { id, reversed } of path) {
    // … 5 lines
    for (let i = out.length && base > 0 ? 1 : 0; i <= n; i++) {
      // … 2 lines
      const deck = elevation.onSegment(id, p.x, p.y);
      // … 1 lines
      out.push({ s: base + along, x: p.x, y: p.y, deck, ground: g, mode: buildModeAt(seg.structure, deck - g) });
    }
  // … 1 lines
  }
// … 1 lines
}
