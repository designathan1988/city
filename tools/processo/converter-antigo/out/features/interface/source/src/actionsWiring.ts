// The code of src/actionsWiring.ts that is the feature's (the rest of the file is shared).

// src/actionsWiring.ts:33-55 slice of src/actionsWiring.ts#ActionsDeps (shared): the feature's statements; "…" marks code left out
export interface ActionsDeps {
  // … 6 lines
  view(): Viewport;
// … 14 lines
}

// src/actionsWiring.ts:89-146 slice of src/actionsWiring.ts#createActions.shootAt (shared): the feature's statements; "…" marks code left out
  function shootAt(sx: number, sy: number): boolean {
    // … 2 lines
    const under = view.toWorldAt(sx, sy, ground(view.toWorld(sx, sy, w, h)), w, h);
    // … 1 lines
    for (const p of sim.pedViews) {
      if (Math.hypot(p.x - under.x, p.y - under.y) >= m(25)) continue;
      // … 4 lines
      for (let k = 0; k <= 37; k++) {
        // … 1 lines
        const q = view.toWorldAt(sx, sy, base + height, w, h);
        const d = Math.hypot(q.x - p.x, q.y - p.y);
        // … 1 lines
        if (d < m(0.42) && (!best || d < best.d)) best = { id: p.id, height, d, x: p.x, y: p.y, heading: p.heading };
      }
    }
    if (!best) {
      // The bodies on the ground, alive or dead, along the line of sight
      const lo = ground(under) - m(0.3), hi = lo + m(5);
      const a = view.toWorldAt(sx, sy, hi, w, h), b = view.toWorldAt(sx, sy, lo, w, h);
      const hit = scene.shootBody([a.x, a.y, hi], [b.x, b.y, lo]);
      if (hit && hit !== 'hit') {
        const len = Math.hypot(b.x - a.x, b.y - a.y) || 1;
        void sim.act({ kind: 'shot', ped: hit.alive, part: hit.part, fromX: b.x - ((b.x - a.x) / len) * m(10), fromY: b.y - ((b.y - a.y) / len) * m(10) }).then((done) => {
          const shot = done as { severed: Parameters<SceneHandle['wound']>[5] } | null;
          if (shot) { scene.wound(b.x, b.y, lo + m(0.4), (b.x - a.x) / len, (b.y - a.y) / len, shot.severed); deps.redraw(); }
        });
      }
      if (hit) { deps.redraw(); return true; }
      if (shootVehicle(a, b, hi, lo)) { deps.redraw(); return true; }
      deps.hint('hint.shoot.miss');
      return false;
    }
    // … 1 lines
    const gz = ground(best);
    const farther = view.toWorldAt(sx, sy - 40, gz, w, h), nearer = view.toWorldAt(sx, sy + 40, gz, w, h);
    let dx = farther.x - nearer.x, dy = farther.y - nearer.y;
    const len = Math.hypot(dx, dy) || 1;
    dx /= len; dy /= len;
    // … 1 lines
    const at = view.toWorldAt(sx, sy, ground(best) + best.height, w, h);
    const side = (at.x - best.x) * -Math.sin(best.heading) + (at.y - best.y) * Math.cos(best.heading);
    const part: BodyPart = best.height > m(1.5) ? 'head'
      : best.height > m(0.9) ? (Math.abs(side) > m(0.17) ? (side > 0 ? 'armL' : 'armR') : 'torso')
        : side >= 0 ? 'legL' : 'legR';
    const shotAt = best;
    // … 1 lines
    void sim.act({ kind: 'shot', ped: shotAt.id, part, fromX: shotAt.x - dx * m(10), fromY: shotAt.y - dy * m(10) }).then((done) => {
      // … 2 lines
      scene.wound(shotAt.x, shotAt.y, ground(shotAt) + shotAt.height, dx, dy, shot.severed);
    // … 1 lines
    });
  // … 1 lines
  }

// src/actionsWiring.ts:448-453 slice of src/actionsWiring.ts#createActions.buildingsUnderRay (shared): the feature's statements; "…" marks code left out
  function buildingsUnderRay(sx: number, sy: number, groundZ: number): Building[] {
    // … 2 lines
    const high = view.toWorldAt(sx, sy, groundZ + RAY_ABOVE, w, h), low = view.toWorldAt(sx, sy, groundZ - RAY_BELOW, w, h);
    return doc.buildings.near(Math.min(high.x, low.x), Math.min(high.y, low.y), Math.max(high.x, low.x), Math.max(high.y, low.y));
  }

// src/actionsWiring.ts:456-472 slice of src/actionsWiring.ts#createActions.rayOnBuilding (shared): the feature's statements; "…" marks code left out
  function rayOnBuilding(b: Building, sx: number, sy: number, base: number): (Vec2 & { z: number }) | null {
    // … 6 lines
    for (let h = top + m(1); h >= low; h -= m(0.5)) {
      const p = view.toWorldAt(sx, sy, base + h, w, height);
      const l = worldToLocal(b, p);
      for (const v of volumes) {
        if (l.x < v.x || l.x > v.x + v.w || l.y < v.y || l.y > v.y + v.d) continue;
        if (h <= volumeElevation(b, v, v.base + v.storeys.length) && h >= volumeElevation(b, v, v.base)) return { x: p.x, y: p.y, z: base + h };
      }
    }
  // … 1 lines
  }
