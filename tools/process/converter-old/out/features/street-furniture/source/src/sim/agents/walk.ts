// The code of src/sim/agents/walk.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/walk.ts:1538-1880 slice of src/sim/agents/walk.ts#stepWalkers (shared): the feature's statements; "…" marks code left out
function stepWalkers(w: SimWorld): void {
  // … 70 lines
  for (const p of s.walkers) {
    // … 109 lines
    for (let k = 0; k < posts.length; k++) {
      // … 1 lines
      const rx = post.x - p.x, ry = post.y - p.y;
      const along = rx * f.tx + ry * f.ty - SIGNAL_POST_RADIUS + BODY;
      if (along + 2 * SIGNAL_POST_RADIUS <= 0 || along > LOOK) continue;
      addOther(Math.max(0.01, along), p.d - rx * f.ty + ry * f.tx, false, SIGNAL_POST_RADIUS, null, false);
    }
  // … 152 lines
  }
// … 1 lines
}
