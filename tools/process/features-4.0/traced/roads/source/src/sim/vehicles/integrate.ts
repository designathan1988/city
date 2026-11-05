// The code of src/sim/vehicles/integrate.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/integrate.ts:18-135 slice of src/sim/vehicles/integrate.ts#integrateAll (shared): the feature's statements; "…" marks code left out
export function integrateAll(w: SimWorld): void {
  // … 2 lines
  for (const v of w.vehiclesInIdOrder()) {
    // … 15 lines
    const speedCap = Math.min(wanted, here.speedLimit, curveCap, positioning);
    // … 1 lines
    const next = physicalSpeed(v.driver, v.v, speedCap, v.accel, v.constraints.obstacles, DT);
    const ds = Math.max(0, 0.5 * (v.v + next) * DT);
    // … 5 lines
    v.accel = (next - v.v) / DT;
    v.v = next;
    v.s += ds;
    // … 3 lines
    if (v.lateral !== 0) {
      v.lateralTravelled += ds;
    // … 3 lines
    } else {
    // … 1 lines
    }
    // … 4 lines
    let travelled = ds;
    // … 3 lines
    while (guard++ < 16) {
      // … 1 lines
      if (!current || v.s <= current.length) break;
      const carried = v.s - current.length;
      if (!advance(w, v, carried)) {
        // Nowhere to go: hold at the end rather than vanish.
        travelled = Math.max(0, travelled - carried);
        discardUntravelledFromNewClearances(v, clearancesAtStart, carried);
        v.s = current.length;
        v.v = 0;
        break;
      }
    }
    // … 2 lines
    if (finalLane && v.s > finalLane.length) {
      const discarded = v.s - finalLane.length;
      travelled = Math.max(0, travelled - discarded);
      discardUntravelledFromNewClearances(v, clearancesAtStart, discarded);
      v.s = finalLane.length;
      v.v = 0;
    }
    // … 2 lines
    advanceClearanceTokens(w, v, travelled, clearancesAtStart);
    // … 1 lines
    if (finalLane?.kind === 'connector' && v.claims.length) {
      w.claims.releasePassed(
        v.id,
        finalLane.id,
        v.s - v.archetype.length / 2,
        bodyClassOfArchetype(v.archetype),
        w.conflicts,
      );
    }
    // … 1 lines
    if (travelled > m(0.008)) {
      v.lastMovedTick = w.clock.tick;
      v.waited = 0;
    } else {
      v.waited += DT;
    }
    // … 2 lines
    if (
      finalLane?.kind === 'link' &&
      v.v < m(0.12) &&
      finalLane.length - v.s <= v.driver.s0 + 0.5
    ) {
      v.rorStopped += DT;
      if (v.rorStopped >= 1.0) v.rorCredit = true;
    } else if (v.v > 1) {
      v.rorStopped = 0;
      v.rorCredit = false;
    }
  // … 3 lines
  }
// … 8 lines
}

// src/sim/vehicles/integrate.ts:138-189 slice of src/sim/vehicles/integrate.ts#applyLaneChange (shared): the feature's statements; "…" marks code left out
function applyLaneChange(w: SimWorld, v: Vehicle): ReturnType<SimWorld['lanelet']> {
  // … 6 lines
  if (!lane || lane.kind !== 'link') return undefined;
  // … 2 lines
  if (v.s > lane.length) return undefined;
  // … 3 lines
  if (!from || from.segment !== lane.segment || from.from !== lane.from || from.to !== lane.to ||
      Math.abs((from.laneIndex ?? 0) - (lane.laneIndex ?? 0)) !== 1) return undefined;
  if (from) {
    // … 2 lines
    const byCentre = v.s - half >= 0 && v.s - half <= from.length;
    const frame = from.centre.sampleAt(byCentre ? v.s - half : Math.min(v.s, from.length));
    // … 1 lines
    const was = { p: addScaled(frame.p, perp(frame.t), v.lateral), t: frame.t };
    // … 2 lines
    const anchor = lane.centre.closestPoint(was.p);
    const front = byCentre ? anchor.s + half : anchor.s;
    // … 1 lines
    if (front > lane.length) return undefined;
    // … 1 lines
    v.s = front;
    const now = lane.centre.sampleAt(anchor.s);
    v.lateral = dot(sub(was.p, now.p), perp(now.t));
    // … 6 lines
    w.addShadow(v, from.id, sOnOld - v.s, Math.abs(v.lateral) / 2);
  }
// … 10 lines
}

// src/sim/vehicles/integrate.ts:212-238 slice of src/sim/vehicles/integrate.ts#advanceClearanceTokens (shared): the feature's statements; "…" marks code left out
function advanceClearanceTokens(
// … 4 lines
): void {
  // … 3 lines
  for (const token of v.clearingConnectors) {
    // … 4 lines
    if (token.distanceBeyondExit + CLEARANCE_EPSILON >= v.archetype.length) {
      w.claims.releaseConnector(v.id, connector.id, w.conflicts);
    // … 1 lines
    }
    // … 1 lines
    const centreOnConnector =
      connector.length + token.distanceBeyondExit - v.archetype.length / 2;
    w.claims.releasePassed(v.id, connector.id, centreOnConnector,
      bodyClassOfArchetype(v.archetype), w.conflicts);
  // … 1 lines
  }
// … 1 lines
}

// src/sim/vehicles/integrate.ts:241-277 slice of src/sim/vehicles/integrate.ts#advance (shared): the feature's statements; "…" marks code left out
function advance(w: SimWorld, v: Vehicle, carried: number): boolean {
  // … 4 lines
  if (current?.kind === 'link' && v.admittedConnector) {
    const admitted = w.connector(v.admittedConnector);
    if (!admitted || admitted.fromLane !== v.lanelet) return false;
    nextId = admitted.id;
    if (v.route[1] !== nextId) {
      const pinned = [v.lanelet, nextId, admitted.toLane];
      let expected = admitted.toLane;
      for (const reservedId of v.reservedConnectors) {
        const reserved = w.connector(reservedId);
        if (!reserved || reserved.fromLane !== expected) break;
        pinned.push(reserved.id, reserved.toLane);
        expected = reserved.toLane;
      }
      v.route = pinned;
      w.noteRoute(v);
    }
  }
  // … 1 lines
  const next = nextId === undefined ? undefined : w.lanelet(nextId);
  // … 1 lines
  if (!next || w.rt(next.id).ghost) {
    // The route is stale or the lanelet retired.
    if (v.admittedConnector) return false;
    planFrom(w, v);
    // A freshly planned connector has not passed admission yet.
    return false;
  }
  // … 1 lines
  if (next.kind === 'connector' && v.admittedConnector !== next.id) return false;
  // … 1 lines
  return advanceTo(w, v, next.id, carried);
}

// src/sim/vehicles/integrate.ts:279-326 slice of src/sim/vehicles/integrate.ts#advanceTo (shared): the feature's statements; "…" marks code left out
function advanceTo(w: SimWorld, v: Vehicle, target: string, carried: number): boolean {
  // … 6 lines
  if (current?.kind === 'link' && lane.kind === 'connector') {
    v.desiredLane = null;
    v.movementIntent = null;
  }
  if (current?.kind === 'connector' && lane.kind === 'link') {
    v.desiredLane = null;
    v.movementIntent = null;
  }
  if (current) v.rearPath = [current.id, ...v.rearPath].slice(0, 32);
  if (
    current?.kind === 'connector' &&
    lane.kind === 'link'
  ) {
    const existing = v.clearingConnectors.find((token) => token.connector === current.id);
    if (existing) existing.distanceBeyondExit = Math.max(existing.distanceBeyondExit, carried);
    else {
      v.clearingConnectors.push({
        connector: current.id,
        distanceBeyondExit: carried,
      });
      w.noteHolder(v);
    }
    // Admission now describes only the movement the front is about to enter.
    if (v.admittedConnector === current.id) v.admittedConnector = null;
  }
  // … 11 lines
  if (lane.kind === 'link') {
    if (v.reservedConnectors.length === 0) v.firstRequestTick = null;
  }
// … 1 lines
}
