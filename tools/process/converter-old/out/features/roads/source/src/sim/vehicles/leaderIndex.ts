// The code of src/sim/vehicles/leaderIndex.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/leaderIndex.ts:14-81 slice of src/sim/vehicles/leaderIndex.ts#findLeader (shared): the feature's statements; "…" marks code left out
export function findLeader(w: SimWorld, v: Vehicle, out?: MutableObstacle | null): Obstacle | null {
  // … 6 lines
  if (idx >= 0 && idx + 1 < rt.order.length) {
    // … 8 lines
    if (lead) {
      // … 5 lines
      if (!found || gap < bestGap) { bestGap = gap; bestSpeed = lead.v; bestAccel = lead.accel; found = true; }
    }
  }
  for (const shadow of shadowsAhead(w, v.lanelet, v.s, v.id)) {
    // … 1 lines
    if (!found || gap < bestGap) { bestGap = gap; bestSpeed = shadow.vehicle.v; bestAccel = shadow.vehicle.accel; found = true; }
  }
  if (found) return obstacleInto(out, bestGap, bestSpeed, 'vehicle', bestAccel);
  // … 5 lines
  let dist = here.length - v.s;
  // … 6 lines
  for (const nextId of upcoming) {
    if (dist > horizon) return null;
    // … 8 lines
    if (tail) {
      const gap = dist + tail.s - tail.archetype.length;
      if (!found || gap < bestGap) { bestGap = gap; bestSpeed = tail.v; bestAccel = tail.accel; found = true; }
    }
    for (const shadow of shadowsAhead(w, nextId, -Infinity, v.id)) {
      const gap = dist + shadow.rear;
      if (!found || gap < bestGap) { bestGap = gap; bestSpeed = shadow.vehicle.v; bestAccel = shadow.vehicle.accel; found = true; }
    }
    if (found) return obstacleInto(out, bestGap, bestSpeed, 'vehicle', bestAccel);
    // … 2 lines
    dist += lanelet.length;
  }
// … 2 lines
}

// src/sim/vehicles/leaderIndex.ts:99-115 slice of src/sim/vehicles/leaderIndex.ts#shadowsAhead (shared): the feature's statements; "…" marks code left out
function shadowsAhead(
  // … 1 lines
  laneId: LaneletId,
// … 2 lines
): { vehicle: Vehicle; rear: number }[] {
  // … 1 lines
  for (const id of w.rt(laneId).shadows) {
    if (id === self) continue;
    const other = w.veh(id);
    if (!other?.shadow || other.shadow.lanelet !== laneId) continue;
    const front = other.s + other.shadow.offset;
    if (front <= s) continue;
    out.push({ vehicle: other, rear: front - other.archetype.length });
  }
// … 1 lines
}

// src/sim/vehicles/leaderIndex.ts:118-165 slice of src/sim/vehicles/leaderIndex.ts#divergeObstacle (shared): the feature's statements; "…" marks code left out
export function divergeObstacle(w: SimWorld, v: Vehicle): Obstacle | null {
  // … 3 lines
  let connectorId: LaneletId | undefined;
  // … 1 lines
  if (here.kind === 'connector') {
    connectorId = here.id;
    offset = -v.s;
  } else {
    // Only the vehicle nearest the stop line needs this; everyone behind it follows it.
    if (w.laneHead(here.id)?.id !== v.id) return null;
    connectorId = v.route[1];
    offset = here.length - v.s;
    const horizon = Math.max(m(20), v.v * v.driver.T * 3 + (v.v * v.v) / (2 * v.driver.b));
    if (offset > horizon) return null;
  }
  if (connectorId === undefined) return null;
  const siblings = w.conflicts.divergesOf(connectorId);
  if (!siblings.length) return null;
  // … 3 lines
  for (const d of siblings) {
    const other = w.connector(d.other);
    if (!other) continue;
    const candidates: { vehicle: Vehicle; front: number }[] = [];
    for (const id of w.rt(other.id).order) {
      const o = w.veh(id);
      if (o) candidates.push({ vehicle: o, front: o.s });
    }
    // Bodies whose front is already on the exit lane but whose tail may not be.
    for (const id of w.rt(other.toLane).order) {
      const o = w.veh(id);
      if (o && o.rearPath[0] === other.id) candidates.push({ vehicle: o, front: other.length + o.s });
    }
    for (const { vehicle: o, front } of candidates) {
      if (o.id === v.id) continue;
      const centre = front - o.archetype.length / 2;
      if (centre > d.otherExit(bodyClassOfArchetype(o.archetype), mine)) continue;
      // Whoever is further along the shared start leads.
      if (here.kind === 'connector' && front <= v.s) continue;
      const gap = offset + front - o.archetype.length;
      if (!best || gap < best.gap) best = { gap, speed: o.v, kind: 'vehicle' };
    }
  }
  return best;
}

// src/sim/vehicles/leaderIndex.ts:194-201 slice of src/sim/vehicles/leaderIndex.ts#upcomingLanelets (shared): the feature's statements; "…" marks code left out
function upcomingLanelets(w: SimWorld, v: Vehicle, n: number): LaneletId[] {
  const out: LaneletId[] = [];
  for (let i = 1; i < v.route.length && out.length < n; i++) {
    const id = v.route[i];
    if (id !== undefined && w.lanelet(id)) out.push(id);
  }
  return out;
}
