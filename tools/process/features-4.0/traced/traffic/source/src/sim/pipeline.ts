// The code of src/sim/pipeline.ts that is the feature's (the rest of the file is shared).

// src/sim/pipeline.ts:29-128 slice of src/sim/pipeline.ts#step (shared): the feature's statements; "…" marks code left out
export function step(w: SimWorld, opts: StepOptions = {}): void {
  // … 22 lines
  for (const v of w.vehicles.values()) snapshotInto(v);
  // … 3 lines
  w.signalClock.passes++;
  // … 5 lines
  for (const c of stepping) {
    // … 2 lines
    if (stepController(c, deps) && w.doLess) w.restSignal(c);
  }
  // … 3 lines
  stepDispatch(w, traffic);
  // … 6 lines
  w.meso?.step(w, traffic);
  // … 5 lines
  if (traffic) stepLaneChange(w);
  // … 2 lines
  if (traffic) stepKerbStops(w);
  // … 8 lines
  if (constrained) {
    for (const v of w.vehiclesInIdOrder()) {
      v.constraints = longitudinalConstraints(w, v);
    }
  }
  // … 3 lines
  if (traffic) stepAdmission(w);
  // … 3 lines
  if (traffic) integrateAll(w);
  // … 3 lines
  stepDespawn(w);
// … 16 lines
}

// src/sim/pipeline.ts:137-170 slice of src/sim/pipeline.ts#rebindVehicles (shared): the feature's statements; "…" marks code left out
export function rebindVehicles(w: SimWorld): void {
  for (const v of w.vehiclesReached() ?? w.vehiclesInIdOrder()) {
    if (w.vehicles.get(v.id) !== v) continue;
    // … 3 lines
    const lane = w.lanelet(v.lanelet);
    if (!lane) {
      // The lanelet is gone.
      w.removeVehicle(v);
      continue;
    }
    v.s = Math.min(v.s, lane.length);
    w.enterLanelet(v, v.lanelet, false);
    // … 1 lines
    const admitted = v.admittedConnector ? w.connector(v.admittedConnector) : undefined;
    const admissionStillValid =
      !!admitted &&
      ((lane.kind === 'link' && admitted.fromLane === lane.id) ||
        (lane.kind === 'connector' && admitted.id === lane.id));
    v.clearingConnectors = v.clearingConnectors.filter((token) =>
      Boolean(w.connector(token.connector)),
    );
    if (v.admittedConnector && !admissionStillValid) {
      w.claims.releaseConnector(v.id, v.admittedConnector, w.conflicts);
      v.admittedConnector = null;
    }
    // … 2 lines
    if (!pinReservedRoute(w, v)) repairRoute(w, v);
    v.claims = [...w.claims.points(v.id)];
    v.firstRequestTick = null;
  }
}

// src/sim/pipeline.ts:181-205 slice of src/sim/pipeline.ts#collectGhosts (shared): the feature's statements; "…" marks code left out
function collectGhosts(w: SimWorld): void {
  if (!w.doLess) {
    for (const [id, rt] of [...w.runtime]) {
      // … 1 lines
      if (!w.graph.lanelets.has(id)) {
        if (rt.order.length === 0) { w.runtime.delete(id); tickWork.retired++; }
        else if (!rt.ghost) {
          rt.ghost = true;
          rt.ghostSince = w.clock.tick;
        }
      }
    }
  // … 1 lines
  }
  for (const id of w.ghostCandidates) {
    // … 2 lines
    if (!rt || w.graph.lanelets.has(id)) { w.ghostCandidates.delete(id); continue; }
  // … 5 lines
  }
}

// src/sim/pipeline.ts:208-217 slice of src/sim/pipeline.ts#updateStallCounters (shared): the feature's statements; "…" marks code left out
function updateStallCounters(w: SimWorld): void {
  for (const id of w.occupied) {
    // … 1 lines
    if (!on || on.kind !== 'link' || !on.controlled) continue;
  // … 4 lines
  }
}

// src/sim/pipeline.ts:220-248 slice of src/sim/pipeline.ts#stallCounters (shared): the feature's statements; "…" marks code left out
function stallCounters(w: SimWorld, v: Vehicle): void {
  if (v.v >= m(0.04)) {
    // BOTH counters reset here.
    v.greenStall = 0;
    v.greenDenied = 0;
    return;
  }
  const conn = nextConnector(w, v);
  const c = conn && w.graph.junctions.get(conn.node)?.signalised ? w.controller(conn.node) : undefined;
  const green = conn && c ? signalStateFor(c, conn.group) === 'green' : false;
  const impeded = v.constraints.obstacles.some(
    (o) =>
      // The driver's standstill gap, as everywhere else that reads s0.
      (o.kind === 'vehicle' && o.gap < v.driver.s0 + m(1.2)) ||
      o.kind === 'yield' ||
      o.kind === 'conflict' ||
      o.kind === 'pedestrian' ||
      o.kind === 'spillback' ||
      // Stopped at the kerb on purpose, letting somebody out or in.
      o.kind === 'kerbStop',
  );
  v.greenStall = green && !impeded ? v.greenStall + DT : 0;
  // … 2 lines
  const signalised = conn ? w.graph.junctions.get(conn.node)?.signalised === true : false;
  // … 1 lines
  const heldByDesign = v.constraints.obstacles.some((o) => o.kind === 'spillback' || o.kind === 'kerbStop');
  v.greenDenied = green && signalised && !heldByDesign ? v.greenDenied + DT : 0;
}

// src/sim/pipeline.ts:251-271 slice of src/sim/pipeline.ts#runWatchdog (shared): the feature's statements; "…" marks code left out
function runWatchdog(w: SimWorld): void {
  for (const v of w.junctionHolders().sort((a, b) => a.id - b.id)) {
    // Rear-clearance and ahead-of-route reservations can legitimately remain while a vehicle waits at another signal.
    if (!v.admittedConnector) continue;
    if (w.clock.since(v.lastMovedTick) > STUCK_SECONDS) {
      // Never release a resource while its physical holder is still present.
      if (
        w.auditEnabled &&
        w.auditLevel === 'cheap' &&
        !w.issues.some((i) => i.code === 'staleClaim' && i.subject === String(v.id))
      ) {
        w.report({
          code: 'staleClaim',
          tick: w.clock.tick,
          subject: String(v.id),
          detail: 'admitted vehicle immobile; reservation retained for safety',
        });
      }
    }
  }
}

// src/sim/pipeline.ts:274-334 slice of src/sim/pipeline.ts#ensureVehicleRoutes (shared): the feature's statements; "…" marks code left out
function ensureVehicleRoutes(w: SimWorld): void {
  for (const v of w.vehiclesInIdOrder()) {
    const lane = w.lanelet(v.lanelet);
    if (!lane) continue;
    // … 2 lines
    reconsiderRoute(w, v);
    // … 3 lines
    if (lane.kind === 'link' && v.admittedConnector) {
      const admitted = w.connector(v.admittedConnector);
      if (admitted?.fromLane === lane.id) {
        if (v.route[0] !== lane.id || v.route[1] !== admitted.id) {
          if (!pinReservedRoute(w, v)) {
            v.route = [lane.id, admitted.id, admitted.toLane];
            w.noteRoute(v);
            repairRoute(w, v);
          }
        }
        continue;
      }
      w.claims.releaseConnector(v.id, v.admittedConnector, w.conflicts);
      v.claims = [...w.claims.points(v.id)];
      v.admittedConnector = null;
    }
    // … 2 lines
    if (v.route.length === 1 && v.route[0] === lane.id && lane.kind === 'link' &&
        v.movementIntent === null && v.desiredLane === null && w.graph.exitsOf(lane.id).length === 0 &&
        (v.destination === lane.id ||
          (v.destination === null && w.graph.siblingLanes(lane.id).every((id) => w.graph.exitsOf(id).length === 0)))) continue;
    // … 1 lines
    const next = v.route[1];
    const connected =
      v.route[0] === lane.id &&
      next !== undefined &&
      (lane.kind === 'link'
        ? w.connector(next)?.fromLane === lane.id
        : w.connector(lane.id)?.toLane === next);
    if (!connected) {
      if (!pinReservedRoute(w, v)) repairRoute(w, v);
      continue;
    }
    // … 2 lines
    if (v.route.length <= 1 && w.graph.exitsOf(lane.id).some((id) =>
      (w.connector(id)?.maxBodyClass ?? -1) >= bodyClassOfArchetype(v.archetype))) {
      planFrom(w, v);
      continue;
    }
    // … 2 lines
    const tail = w.lanelet(v.route[v.route.length - 1] ?? '');
    if (tail && tail.kind === 'link' && tail.id !== v.destination && tail.id !== lane.id &&
        tail.length < v.archetype.length + Math.max(JAM_GAP, v.driver.s0) &&
        w.graph.exitsOf(tail.id).length > 0) {
      extend(w, v);
    }
  }
}

// src/sim/pipeline.ts:337-350 slice of src/sim/pipeline.ts#reconcileReservedChain (shared): the feature's statements; "…" marks code left out
function reconcileReservedChain(w: SimWorld, v: Vehicle): boolean {
  const lane = w.lanelet(v.lanelet);
  if (!lane) return false;
  // … 1 lines
  const was = w.reservationsChecked.get(v);
  if (was && was.lanelet === v.lanelet && was.admitted === v.admittedConnector && was.reserved === v.reservedConnectors && was.reservedLength === v.reservedConnectors.length
    && was.route === v.route && was.routeLength === v.route.length && was.topology === w.topologyLog.state) return true;
  const valid = reservedChainValid(w, v, lane);
  if (valid) {
    w.reservationsChecked.set(v, { lanelet: v.lanelet, admitted: v.admittedConnector, reserved: v.reservedConnectors, reservedLength: v.reservedConnectors.length,
      route: v.route, routeLength: v.route.length, topology: w.topologyLog.state });
  } else w.reservationsChecked.delete(v);
  return valid;
}

// src/sim/pipeline.ts:352-409 slice of src/sim/pipeline.ts#reservedChainValid (shared): the feature's statements; "…" marks code left out
function reservedChainValid(w: SimWorld, v: Vehicle, lane: NonNullable<ReturnType<SimWorld['lanelet']>>): boolean {
  // … 1 lines
  if (lane.kind === 'connector') {
    expectedFrom = w.connector(lane.id)?.toLane;
  } else if (v.admittedConnector) {
    const admitted = w.connector(v.admittedConnector);
    expectedFrom = admitted?.fromLane === lane.id ? admitted.toLane : undefined;
  } else {
    expectedFrom = lane.id;
  }
  // … 1 lines
  let valid = expectedFrom !== undefined;
  for (const id of v.reservedConnectors) {
    const connector = w.connector(id);
    if (!connector || connector.fromLane !== expectedFrom) {
      valid = false;
      break;
    }
    expectedFrom = connector.toLane;
  }
  // … 2 lines
  const root = lane.kind === 'connector'
    ? w.connector(lane.id)
    : v.admittedConnector
      ? w.connector(v.admittedConnector)
      : v.reservedConnectors[0]
        ? w.connector(v.reservedConnectors[0])
        : undefined;
  if (valid && root) {
    const required = compactReservationChain(w, v, root);
    const expectedReservations =
      lane.kind === 'link' && !v.admittedConnector
        ? required?.map((connector) => connector.id)
        : required?.slice(1).map((connector) => connector.id);
    valid =
      expectedReservations !== undefined &&
      expectedReservations.length === v.reservedConnectors.length &&
      expectedReservations.every((id, index) => v.reservedConnectors[index] === id);
  }
  if (valid) return true;
  // … 2 lines
  if (lane.kind === 'connector' || v.clearingConnectors.length > 0) return false;
  // … 1 lines
  for (const id of v.reservedConnectors) {
    w.claims.releaseConnector(v.id, id, w.conflicts);
  }
  v.reservedConnectors = [];
  // … 2 lines
  if (lane.kind === 'link' && v.admittedConnector) {
    w.claims.releaseConnector(v.id, v.admittedConnector, w.conflicts);
    v.admittedConnector = null;
  }
  v.claims = [...w.claims.points(v.id)];
// … 1 lines
}

// src/sim/pipeline.ts:412-450 slice of src/sim/pipeline.ts#pinReservedRoute (shared): the feature's statements; "…" marks code left out
function pinReservedRoute(w: SimWorld, v: Vehicle): boolean {
  const lane = w.lanelet(v.lanelet);
  if (!lane) return false;
  // … 1 lines
  const prefix: string[] = [lane.id];
  // … 1 lines
  if (lane.kind === 'connector') {
    const current = w.connector(lane.id);
    if (!current) return false;
    expectedLink = current.toLane;
    prefix.push(expectedLink);
  } else {
    expectedLink = lane.id;
    if (v.admittedConnector) {
      const admitted = w.connector(v.admittedConnector);
      if (!admitted || admitted.fromLane !== expectedLink) return false;
      prefix.push(admitted.id, admitted.toLane);
      expectedLink = admitted.toLane;
    }
  }
  // … 1 lines
  if (!v.admittedConnector && !v.reservedConnectors.length) return false;
  for (const id of v.reservedConnectors) {
    const connector = w.connector(id);
    if (!connector || connector.fromLane !== expectedLink) return false;
    if (prefix[prefix.length - 1] !== connector.fromLane) return false;
    prefix.push(connector.id, connector.toLane);
    expectedLink = connector.toLane;
  }
  // … 1 lines
  const suffixAt = v.route.indexOf(expectedLink);
  const suffix = suffixAt >= 0 ? v.route.slice(suffixAt + 1) : [];
  v.route = [...prefix, ...suffix];
  w.noteRoute(v);
  // … 1 lines
  if (lane.kind === 'link') repairRoute(w, v);
  else if (cutAtBreak(w, v)) extend(w, v);
// … 1 lines
}
