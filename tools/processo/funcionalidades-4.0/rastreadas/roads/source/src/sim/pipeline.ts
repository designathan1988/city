// The code of src/sim/pipeline.ts that is the feature's (the rest of the file is shared).

// src/sim/pipeline.ts:29-128 slice of src/sim/pipeline.ts#step (shared): the feature's statements; "…" marks code left out
export function step(w: SimWorld, opts: StepOptions = {}): void {
  // … 13 lines
  if (!w.topologyDeferred && w.topologyRevision !== w.net.trafficRevision) {
    w.rebuildTopology();
    rebindAgents(w);
  }
// … 81 lines
}

// src/sim/pipeline.ts:137-170 slice of src/sim/pipeline.ts#rebindVehicles (shared): the feature's statements; "…" marks code left out
export function rebindVehicles(w: SimWorld): void {
  for (const v of w.vehiclesReached() ?? w.vehiclesInIdOrder()) {
    // … 10 lines
    v.s = Math.min(v.s, lane.length);
    // … 3 lines
    const admissionStillValid =
      !!admitted &&
      ((lane.kind === 'link' && admitted.fromLane === lane.id) ||
        (lane.kind === 'connector' && admitted.id === lane.id));
    // … 3 lines
    if (v.admittedConnector && !admissionStillValid) {
      w.claims.releaseConnector(v.id, v.admittedConnector, w.conflicts);
      v.admittedConnector = null;
    }
  // … 5 lines
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
  // … 7 lines
  const c = conn && w.graph.junctions.get(conn.node)?.signalised ? w.controller(conn.node) : undefined;
  const green = conn && c ? signalStateFor(c, conn.group) === 'green' : false;
  // … 11 lines
  v.greenStall = green && !impeded ? v.greenStall + DT : 0;
  // … 2 lines
  const signalised = conn ? w.graph.junctions.get(conn.node)?.signalised === true : false;
  // … 2 lines
  v.greenDenied = green && signalised && !heldByDesign ? v.greenDenied + DT : 0;
}

// src/sim/pipeline.ts:274-334 slice of src/sim/pipeline.ts#ensureVehicleRoutes (shared): the feature's statements; "…" marks code left out
function ensureVehicleRoutes(w: SimWorld): void {
  for (const v of w.vehiclesInIdOrder()) {
    // … 8 lines
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
    // … 2 lines
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
    // … 3 lines
    if (tail && tail.kind === 'link' && tail.id !== v.destination && tail.id !== lane.id &&
        tail.length < v.archetype.length + Math.max(JAM_GAP, v.driver.s0) &&
        w.graph.exitsOf(tail.id).length > 0) {
      extend(w, v);
    }
  }
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
    // … 1 lines
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
  // … 7 lines
  if (lane.kind === 'link' && v.admittedConnector) {
    w.claims.releaseConnector(v.id, v.admittedConnector, w.conflicts);
    v.admittedConnector = null;
  }
// … 2 lines
}

// src/sim/pipeline.ts:412-450 slice of src/sim/pipeline.ts#pinReservedRoute (shared): the feature's statements; "…" marks code left out
function pinReservedRoute(w: SimWorld, v: Vehicle): boolean {
  // … 3 lines
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
  // … 2 lines
  for (const id of v.reservedConnectors) {
    // … 1 lines
    if (!connector || connector.fromLane !== expectedLink) return false;
    if (prefix[prefix.length - 1] !== connector.fromLane) return false;
    prefix.push(connector.id, connector.toLane);
    expectedLink = connector.toLane;
  }
  // … 1 lines
  const suffixAt = v.route.indexOf(expectedLink);
  const suffix = suffixAt >= 0 ? v.route.slice(suffixAt + 1) : [];
  v.route = [...prefix, ...suffix];
  // … 2 lines
  if (lane.kind === 'link') repairRoute(w, v);
  else if (cutAtBreak(w, v)) extend(w, v);
// … 1 lines
}
