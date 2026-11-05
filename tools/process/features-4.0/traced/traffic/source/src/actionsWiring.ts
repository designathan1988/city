// The code of src/actionsWiring.ts that is the feature's (the rest of the file is shared).

// src/actionsWiring.ts:28-28 slice of src/actionsWiring.ts#VEHICLE_HALF_SPAN (shared): the feature's statements; "…" marks code left out
const VEHICLE_HALF_SPAN = Math.max(...ARCHETYPES.map((a) => Math.hypot(a.length / 2, Math.max(a.width / 2, m(0.35)))));

// src/actionsWiring.ts:149-210 slice of src/actionsWiring.ts#createActions.shootVehicle (shared): the feature's statements; "…" marks code left out
  function shootVehicle(a: Vec2, b: Vec2, hi: number, lo: number): boolean {
    // … 6 lines
    for (const v of sim.vehiclesIn(reach, [], 'traffic')) {
      // … 4 lines
      const two = v.archetype.shape === 'bicycle' || v.archetype.shape === 'motorcycle';
      const L = v.archetype.length / 2, W = Math.max(v.archetype.width / 2, two ? m(0.35) : 0), H = two ? Math.max(v.archetype.height, m(1.75)) : v.archetype.height;
      // … 3 lines
      const lo3 = [-L, -W, 0], hi3 = [L, W, H];
      // … 1 lines
      for (let i = 0; i < 3 && t0 <= t1; i++) {
        const d = p1[i]! - p0[i]!;
        if (Math.abs(d) < 1e-9) { if (p0[i]! < lo3[i]! || p0[i]! > hi3[i]!) t0 = 2; continue; }
        let ta = (lo3[i]! - p0[i]!) / d, tb = (hi3[i]! - p0[i]!) / d;
        if (ta > tb) [ta, tb] = [tb, ta];
        t0 = Math.max(t0, ta); t1 = Math.min(t1, tb);
      }
      if (t0 > t1 || (best && t0 >= best.t)) continue;
      const z = hi + (lo - hi) * t0;
      best = { v, t: t0, x: a.x + (b.x - a.x) * t0, y: a.y + (b.y - a.y) * t0, z, up: (z - g) / H, angle: pose.angle };
    }
    // … 4 lines
    const shape = v.archetype.shape;
    // … 2 lines
    if (shape === 'bicycle' || shape === 'motorcycle') {
      // The rider shot off: thrown down dead, the machine falling over.
      const g = ground({ x, y });
      scene.wound(x, y, g + m(1.2), dirX, dirY, null);
      scene.flingOccupants([{ id: 8_000_000 + v.id, x, y, z: g + m(0.9), heading: best.angle, blastX: x - dirX * m(4), blastY: y - dirY * m(4),
        power: 0.15, kind: 'dead', index: scene.driverBody(v, x, y) }]);
      scene.dropVehicle({ id: v.id, archetype: v.archetype, x, y, angle: best.angle, color, dirX, dirY });
      void sim.act({ kind: 'removeVehicle', id: v.id });
      vehicleShots.delete(v.id);
      return true;
    }
  // … 13 lines
  }

// src/actionsWiring.ts:229-386 slice of src/actionsWiring.ts#createActions.explodeAt (shared): the feature's statements; "…" marks code left out
  function explodeAt(world: Vec2, z: number, b: Building | null, strength: number, quiet = false): void {
    // … 131 lines
    for (const v of sim.vehiclesIn({ x0: world.x - blown, y0: world.y - blown, x1: world.x + blown, y1: world.y + blown }, [], 'traffic')) {
      // … 4 lines
      hit.vehicles.push({ x: p.x, y: p.y, angle: pose.angle, length: v.archetype.length, width: v.archetype.width,
        height: v.archetype.height, color: parseInt(css.replace('#', '').slice(0, 6), 16) || 0x777777, id: v.id, archetype: v.archetype });
      throwAboard(v.id, p.x, p.y, pose.angle, v.archetype.shape === 'bicycle' || v.archetype.shape === 'motorcycle', scene.driverBody(v, pose.p.x, pose.p.y));
    // … 1 lines
    }
  // … 15 lines
  }
