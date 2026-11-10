// The code of src/sim/vehicles/obstacles.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/obstacles.ts:20-56 slice of src/sim/vehicles/obstacles.ts#pedestrianAhead (shared): the feature's statements; "…" marks code left out
function pedestrianAhead(w: SimWorld, v: Vehicle): Obstacle | null {
  // … 6 lines
  if (lane.kind === 'connector') {
    connectorId = lane.id;
    offset = -v.s;
  } else if (v.admittedConnector) {
    connectorId = v.admittedConnector;
    offset = lane.length - v.s;
  } else if (v.route[0] === lane.id && v.route[1] !== undefined && w.connector(v.route[1])) {
    // Not admitted yet: the turn it plans.
    connectorId = v.route[1];
    offset = lane.length - v.s;
  } else {
    return null;
  }
  const conn = w.connector(connectorId);
  if (!conn) return null;
  // … 1 lines
  for (const crossing of crossingIdsAt(w, conn.node)) {
    const span = pedestrianInSpan(w, conn.id, crossing);
    if (!span) continue;
    // While still on a link, keep the whole vehicle behind the near edge of any zebra on the planned turn.
    const buffered = offset - PED_CROSSING_STOP_BUFFER;
    const gap = lane.kind === 'link'
      ? Math.max(0, canStopComfortably(v.driver, v.v, buffered - v.driver.s0) ? buffered : offset)
      : offset + span.along - PED_STOP_MARGIN;
    // Already over the span: stopping there would park on the zebra.
    if (gap < 0) continue;
    if (!best || gap < best.gap) best = { gap: Math.max(0, gap), speed: 0, kind: 'pedestrian' };
  }
  return best;
}

// src/sim/vehicles/obstacles.ts:60-72 slice of src/sim/vehicles/obstacles.ts#crossingIdsAt (shared): the feature's statements; "…" marks code left out
function crossingIdsAt(w: SimWorld, node: NodeId): readonly CrossingId[] {
  let memo = CROSSING_IDS.get(w);
  if (!memo) CROSSING_IDS.set(w, memo = { seen: null, byNode: new Map() });
  if (memo.seen !== w.topologyLog.state) {
    const changes = w.topologyLog.since(memo.seen);
    memo.seen = w.topologyLog.state;
    if (changes === null) memo.byNode.clear();
    else for (const change of changes) for (const junction of change.junctions) memo.byNode.delete(junction);
  }
  let ids = memo.byNode.get(node);
  if (!ids) memo.byNode.set(node, ids = (w.doc.node(node)?.incident ?? []).map((segment) => makeCrossingId(node, segment)));
  return ids;
}

// src/sim/vehicles/obstacles.ts:75-103 slice of src/sim/vehicles/obstacles.ts#mergeObstacle (shared): the feature's statements; "…" marks code left out
function mergeObstacle(w: SimWorld, v: Vehicle): Obstacle | null {
  // … 1 lines
  const connectorId = lane?.kind === 'connector' ? lane.id : v.admittedConnector;
  const conn = connectorId ? w.connector(connectorId) : undefined;
  if (!conn) return null;
  const mine = mergeRemaining(w, v, conn);
  if (mine === null) return null;
  const junction = w.graph.junctions.get(conn.node);
  if (!junction) return null;
  // … 1 lines
  for (const otherId of junction.connectors) {
    if (otherId === conn.id) continue;
    const other = w.connector(otherId);
    if (!other || other.toLane !== conn.toLane) continue;
    const consider = (id: number): void => {
      const o = w.veh(id);
      if (!o || o.id === v.id) return;
      const theirs = mergeRemaining(w, o, other);
      if (theirs === null || theirs > mine || (theirs === mine && o.id > v.id)) return;
      const gap = mine - theirs - o.archetype.length;
      if (!best || gap < best.gap) best = { gap: Math.max(0, gap), speed: o.v, kind: 'vehicle' };
    };
    for (const id of w.rt(other.id).order) consider(id);
    for (const id of w.rt(other.fromLane).order) {
      if (w.veh(id)?.admittedConnector === other.id) consider(id);
    }
  }
  return best;
}

// src/sim/vehicles/obstacles.ts:109-212 slice of src/sim/vehicles/obstacles.ts#longitudinalConstraints (shared): the feature's statements; "…" marks code left out
export function longitudinalConstraints(w: SimWorld, v: Vehicle): ConstraintSet {
  // … 28 lines
  for (let k = 0; k < Math.min(3, v.route.length); k++) {
    // … 4 lines
    for (let j = 0; j < k; j++) {
      offset += w.lanelet(v.route[j]!)?.length ?? 0;
      if (offset > m(120)) { reached = false; break; }
    }
    if (!reached) break;
    for (const at of closed) {
      // … 1 lines
      if (offset + at <= 0) continue;
      addObstacle(constraints, Math.max(0, offset + at - CROSSING_STOP), 0, 'signal');
    }
  }
  // … 1 lines
  if (anyClosed(w)) {
    // … 1 lines
    for (let k = 0; k < Math.min(3, v.route.length) && offset <= m(120); k++) {
      if (k > 0) offset += w.lanelet(v.route[k - 1]!)?.length ?? 0;
      for (const c of closedOn(w, v.route[k]!)) {
        // Its front over the stretch's start already: it was let through before the stretch closed.
        if (offset + c.from <= 0) continue;
        addObstacle(constraints, Math.max(0, offset + c.from - CLOSURE_STOP), 0, 'signal');
      }
    }
  }
  // … 9 lines
  const dStop = Math.max(0, lane.length - v.s);
  // … 1 lines
  if (lane.kind === 'link' && lane.controlled && !v.admittedConnector) {
    // Rails just short of the stop line: it waits for the junction before them, never on the track
    const track = w.city.transit.crossingNearEnd(lane.id, lane.length);
    if (track !== null && v.s < track) {
      addObstacle(constraints, Math.max(0, track - CROSSING_STOP - v.s), 0, 'signal');
    }
  }
  // … 1 lines
  if (lane.kind === 'link' && lane.controlled) {
    // A vehicle with an admission token is never held at the stop line.
    if (!v.admittedConnector) {
      const conn = nextConnector(w, v);
      const controller = conn ? w.controller(conn.node) : undefined;
      const junction = conn ? w.graph.junctions.get(conn.node) : undefined;

      if (conn && controller && junction?.signalised) {
        const state = signalStateFor(controller, conn.group);
        const adjacentStop = w.crossingSpans.approachStop(lane.id);
        const signalGap = adjacentStop === undefined ? dStop :
          Math.min(dStop, Math.max(0, adjacentStop - v.s));
        // THE DRIVER, not the archetype.
        const mustStop = signalHolds(v, conn.id, state, conn.turn, signalGap);
        if (mustStop) {
          addObstacle(constraints, signalGap, 0, 'signal');
        }
      }
    }
  }
  // … 2 lines
  if (v.route.length <= 1 && lane.kind === 'link') {
    addObstacle(constraints, dStop, 0, 'endOfRoute');
  }
// … 2 lines
}
