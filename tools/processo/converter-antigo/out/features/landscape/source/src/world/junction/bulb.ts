// The code of src/world/junction/bulb.ts that is the feature's (the rest of the file is shared).

// src/world/junction/bulb.ts:104-135 slice of src/world/junction/bulb.ts#clearBulbsOf (shared): the feature's statements; "…" marks code left out
function clearBulbsOf(doc: RoadDoc, net: Network, delta: ReturnType<Network['changesSince']>): number {
  // … 13 lines
  for (const bulb of bulbs) {
    // … 4 lines
    for (const item of doc.landscapeIn(bulb.box)) {
      if (inside(item) && doc.removeLandscape(item.id)) taken++;
    }
  }
// … 8 lines
}
