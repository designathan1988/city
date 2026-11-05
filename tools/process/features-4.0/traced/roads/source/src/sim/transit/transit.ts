// The code of src/sim/transit/transit.ts that is the feature's (the rest of the file is shared).

// src/sim/transit/transit.ts:351-357 slice of src/sim/transit/transit.ts#TransitSim.stopPoint (shared): the feature's statements; "…" marks code left out
  private stopPoint(w: SimWorld, stopId: number): { x: number; y: number; speed: number } | null {
    // … 3 lines
    const p = l.centre.sampleAt(Math.min(l.length, Math.max(0, lane.at))).p;
    return { x: p.x, y: p.y, speed: l.speedLimit };
  }

// src/sim/transit/transit.ts:387-409 slice of src/sim/transit/transit.ts#TransitSim.spawnBus (shared): the feature's statements; "…" marks code left out
  private spawnBus(w: SimWorld, line: TransitLine, bus: Bus): VehicleId | null {
    // … 7 lines
    for (const body of w.bodiesIn(l.id)) if (body.s > start - arch.length * 2 && body.s - body.vehicle.archetype.length < lane.at + arch.length) return null;
    // … 1 lines
    const v0 = l.speedLimit * arch.speedFactor * w.rng.driver.range(DRIVER_NOISE.lo, DRIVER_NOISE.hi);
    const v = createVehicle(w.nextVehicleId++, arch, driver, line.colour, l.id, v0, w.clock.tick);
    v.s = start;
    v.v = 0;
    v.prev = snapshot(v);
    v.errand = null;
    w.vehicles.set(v.id, v);
    w.enterLanelet(v, l.id);
    v.commute = { trip: -1, lanelet: lane.lanelet, at: lane.at + arch.length / 2 };
    v.destination = lane.lanelet;
    planFrom(w, v);
    return v.id;
  }

// src/sim/transit/transit.ts:560-580 slice of src/sim/transit/transit.ts#levelCrossings (shared): the feature's statements; "…" marks code left out
export function levelCrossings(w: SimWorld, points: readonly Vec2[], at: readonly number[]): LevelCrossing[] {
  // … 1 lines
  for (let i = 0; i < points.length; i++) {
    // … 3 lines
    for (const lane of lanesInBox(w, x0, y0, x1, y1)) {
      const c = lane.centre;
      for (let j = 1; j < c.n; j++) {
        const p = c.point(j - 1), q = c.point(j);
        if (Math.max(p.x, q.x) < x0 || Math.min(p.x, q.x) > x1 || Math.max(p.y, q.y) < y0 || Math.min(p.y, q.y) > y1) continue;
        const hit = crossAt(a, b, p, q);
        if (!hit) continue;
        const laneS = c.closestPoint({ x: p.x + (q.x - p.x) * hit.v, y: p.y + (q.y - p.y) * hit.v }).s;
        found.push({ lane: lane.id, j, i, crossing: { lanelet: lane.id, at: laneS, loopS: at[i]! + Math.hypot(b.x - a.x, b.y - a.y) * hit.u } });
      }
    }
  }
// … 2 lines
}
