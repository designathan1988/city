// The code of src/sim/agents/lorryTraffic.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lorryTraffic.ts:44-67 slice of src/sim/agents/lorryTraffic.ts#Lorry (shared): the feature's statements; "…" marks code left out
interface Lorry {
  // … 11 lines
  readonly lane: LaneletId;
// … 10 lines
}

// src/sim/agents/lorryTraffic.ts:218-262 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.call (shared): the feature's statements; "…" marks code left out
  private call(w: SimWorld): void {
    // … 24 lines
    const stop = lane.centre.sampleAt(gate.stopAt).p;
    // … 2 lines
    for (const v of w.vehiclesNear(stop.x, stop.y, CALL_REACH)) {
      if (v.archetype.id !== 'truck' || v.commute || v.kerbStop || v.errand || this.lorries.has(v.id)) continue;
      if (v.lanelet === lane.id && v.s > gate.stopAt) continue;
      const pose = vehiclePose(w, v, 1);
      if (!pose) continue;
      const d = Math.hypot(pose.p.x - stop.x, pose.p.y - stop.y);
      if (d < bestD) { bestD = d; best = v; }
    }
    if (!best) return;
    const v = best, was = v.destination;
    v.commute = { trip: -3, lanelet: lane.id, at: gate.stopAt + v.archetype.length / 2 };
    v.destination = lane.id;
    planFrom(w, v);
    if (v.destination !== lane.id) { v.commute = null; v.destination = was; planFrom(w, v); return; }
    this.lorries.set(v.id, this.make(w, v, yard, dock, ways));
  // … 1 lines
  }

// src/sim/agents/lorryTraffic.ts:349-367 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.join (shared): the feature's statements; "…" marks code left out
  private join(w: SimWorld, lorry: Lorry): void {
    // … 7 lines
    const v0 = l.speedLimit * a.speedFactor * w.rng.driver.range(DRIVER_NOISE.lo, DRIVER_NOISE.hi);
    const road = createVehicle(id, a, body.driver, body.color, l.id, v0, w.clock.tick);
    road.s = Math.min(l.length, lorry.joinAt + a.length / 2);
    road.v = JOIN_SPEED;
    road.prev = snapshot(road);
    w.vehicles.set(road.id, road);
    w.enterLanelet(road, l.id);
    road.destination = chooseVehicleDestination(w, l.id, bodyClassOfArchetype(a));
    planFrom(w, road);
  // … 1 lines
  }

// src/sim/agents/lorryTraffic.ts:429-452 slice of src/sim/agents/lorryTraffic.ts#closures (shared): the feature's statements; "…" marks code left out
export function closures(w: SimWorld, poses: readonly Waypoint[], owner: number, rear: number, length: number, width: number): Closure[] {
  // … 4 lines
  const spans = new Map<LaneletId, { from: number; to: number }>();
  // … 1 lines
  for (const lane of lanesInBox(w, x0 - pad, y0 - pad, x1 + pad, y1 + pad)) {
    for (let i = 0; i < poses.length; i += 2) {
      // … 3 lines
      for (const [u, v] of [[-back, -hw], [-back, hw], [front, -hw], [front, hw], [(front - back) / 2, -hw], [(front - back) / 2, hw]] as const) {
        // … 1 lines
        const hit = lane.centre.closestPoint(q);
        if (hit.distance > LANE_HALF) continue;
        const span = spans.get(lane.id);
        if (span) { span.from = Math.min(span.from, hit.s); span.to = Math.max(span.to, hit.s); }
        else spans.set(lane.id, { from: hit.s, to: hit.s });
      }
    }
  }
  return [...spans].map(([lanelet, span]) => ({ lanelet, from: Math.max(0, span.from - CLOSURE_MARGIN), to: span.to + CLOSURE_MARGIN, owner }));
}
