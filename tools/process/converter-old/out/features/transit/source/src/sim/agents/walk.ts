// The code of src/sim/agents/walk.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/walk.ts:1538-1880 slice of src/sim/agents/walk.ts#stepWalkers (shared): the feature's statements; "…" marks code left out
function stepWalkers(w: SimWorld): void {
  // … 59 lines
  for (const discs of s.walkers.length ? w.city.transit.trainZones(true, walkersBox) : []) {
    let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
    for (const d of discs) { x0 = Math.min(x0, d.x - d.r); y0 = Math.min(y0, d.y - d.r); x1 = Math.max(x1, d.x + d.r); y1 = Math.max(y1, d.y + d.r); }
    // `on` empty: whoever is on a train's ground walks off it (`inside` below).
    carZones.push({ x0, y0, x1, y1, discs, on: TRAIN_GROUND, crossingOnly: false });
  }
// … 276 lines
}
