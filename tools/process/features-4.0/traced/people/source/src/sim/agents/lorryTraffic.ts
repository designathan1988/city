// The code of src/sim/agents/lorryTraffic.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lorryTraffic.ts:381-397 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.mayGo (shared): the feature's statements; "…" marks code left out
  private mayGo(w: SimWorld, lorry: Lorry, shunt: Shunt): boolean {
    // … 3 lines
    for (let s = shunt.s; s <= Math.min(shunt.length, shunt.s + m(4)) && clear; s += m(1)) {
      const q = shunt.poseAt(s);
      const c = Math.cos(q.angle), sn = Math.sin(q.angle);
      for (let k = -2; k <= 2 && clear; k++) {
        const x = q.x + c * k * (a.length / 5), y = q.y + sn * k * (a.length / 5);
        // The people near the point (`PeopleBridge.anyoneWithin`, the walkers' grid), not every person of the map.
        if (w.pedEngine.bridge.anyoneWithin(w, x, y, r, null)) clear = false;
      }
    }
    if (clear) { lorry.waited = 0; return true; }
  // … 2 lines
  }
