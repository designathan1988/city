// The code of src/world/junction/bulb.ts that is the feature's (the rest of the file is shared).

// src/world/junction/bulb.ts:104-135 slice of src/world/junction/bulb.ts#clearBulbsOf (shared): the feature's statements; "…" marks code left out
function clearBulbsOf(doc: RoadDoc, net: Network, delta: ReturnType<Network['changesSince']>): number {
  // … 13 lines
  for (const bulb of bulbs) {
    // … 1 lines
    for (const pole of doc.polesIn(bulb.box)) {
      if (inside(pole)) { doc.removePole(pole.id); taken++; }
    }
  // … 3 lines
  }
// … 8 lines
}
