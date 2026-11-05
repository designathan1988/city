// The code of src/sim/intersections/admission.ts that is the feature's (the rest of the file is shared).

// src/sim/intersections/admission.ts:24-31 slice of src/sim/intersections/admission.ts#Request (shared): the feature's statements; "…" marks code left out
interface Request {
  // … 1 lines
  readonly conn: Connector;
// … 4 lines
}

// src/sim/intersections/admission.ts:44-47 slice of src/sim/intersections/admission.ts#ConnectorReservation (shared): the feature's statements; "…" marks code left out
interface ConnectorReservation {
  readonly connector: Connector;
// … 1 lines
}

// src/sim/intersections/admission.ts:98-214 slice of src/sim/intersections/admission.ts#admit (shared): the feature's statements; "…" marks code left out
function admit(w: SimWorld, banker: BankerPass): void {
  // … 3 lines
  for (const node of w.doLess ? w.junctionsApproached() : w.junctionNodesInOrder()) {
    const junction = w.graph.junctions.get(node);
    if (!junction) continue;
    // … 1 lines
    for (const laneId of junction.inbound) {
      const head = w.laneHead(laneId);
      if (!head) continue;
      // A vehicle already granted a movement never competes again.
      if (head.admittedConnector) continue;

      const conn = nextConnector(w, head);
      if (!conn) continue;

      const lane = w.lanelet(laneId);
      if (!lane) continue;
      const d = lane.length - head.s;
      // Waiting before rails just short of the line, it asks from there.
      const track = w.city.transit.crossingNearEnd(laneId, lane.length);
      const before = track === null ? 0 : lane.length - track + CROSSING_STOP + head.driver.s0 + m(2);
      if (d > Math.max(REQUEST_MIN_DISTANCE, head.v * REQUEST_TIME, before)) continue;

      const reservedCurrent = head.reservedConnectors[0] === conn.id;
      // A declared maximum claim pins the route.
      if (head.reservedConnectors.length > 0 && !reservedCurrent) {
        addObstacle(head.constraints, Math.max(0, d), 0, 'conflict');
        continue;
      }

      if (head.firstRequestTick === null) head.firstRequestTick = w.clock.tick;

      requests.push({
        v: head,
        conn,
        d,
        requestTick: head.firstRequestTick,
        row: rightOfWay(w, node, head, conn, d),
      });
    }
  }
  // … 4 lines
  for (const r of requests) {
    const list = pending.get(r.conn.node);
    if (list) list.push(r);
    else pending.set(r.conn.node, [r]);
  }
  // … 7 lines
  for (const r of requests) {
    // … 1 lines
    if (verdict.ok) {
      // … 1 lines
      if (verdict.reserveOnly) {
        // … 1 lines
        if (verdict.reservations) {
          r.v.reservedConnectors = reservations.map(
            (reservation) => reservation.connector.id,
          );
        }
      // … 6 lines
      }
      // … 2 lines
      if (current) {
        w.claims.grantAll(
          r.v.id,
          current.connector.id,
          current.points,
          w.clock.tick,
        );
      }
      // … 2 lines
      if (r.v.reservedConnectors[0] === r.conn.id) {
        r.v.reservedConnectors.shift();
      } else {
        r.v.reservedConnectors = reservations
          .slice(1)
          .map((reservation) => reservation.connector.id);
      }
      r.v.admittedConnector = r.conn.id;
      // … 1 lines
      w.mergeTurn.set(r.conn.toLane, r.conn.fromLane);
      // … 1 lines
      granted(r.v, r.conn.id);
      // … 6 lines
      w.lastAdmission.set(r.conn.node, w.clock.tick);
    } else {
      // … 2 lines
      const held = r.d - PED_CROSSING_STOP_BUFFER;
      const pedestrianGap = canStopComfortably(r.v.driver, r.v.v, held - r.v.driver.s0) ? held : r.d;
      addObstacle(r.v.constraints, Math.max(0, verdict.reason === 'pedestrian' ? pedestrianGap : r.d), 0, verdict.reason ?? 'signal');
    }
  }
}

// src/sim/intersections/admission.ts:217-257 slice of src/sim/intersections/admission.ts#revokeStaleGrants (shared): the feature's statements; "…" marks code left out
function revokeStaleGrants(w: SimWorld): void {
  // … 2 lines
  for (const v of admitted) {
    // … 3 lines
    if (!lane || lane.kind !== 'link') continue;
    // … 2 lines
    if (!conn || conn.fromLane !== lane.id) continue;
    // … 1 lines
    const head = w.laneHead(lane.id);
    const cutIn = head !== undefined && head !== null && head !== v && head.admittedConnector !== conn.id;
    if (v.v <= CONVOY_ROLLING && (cutIn || !hasDownstreamStorage(w, v, conn) || stoppedForWalker(v, Math.max(0, lane.length - v.s)))) {
      w.claims.releaseConnector(v.id, conn.id, w.conflicts);
      v.claims = [...w.claims.points(v.id)];
      v.admittedConnector = null;
      v.reservedConnectors = [];
      continue;
    }
    const controller = w.controller(conn.node);
    if (!controller || !w.graph.junctions.get(conn.node)?.signalised) continue;
    // … 1 lines
    const state = signalStateFor(controller, conn.group);
    if (state === 'green') continue;
    const d = Math.max(0, lane.length - v.s);
    // … 2 lines
    if (!signalHolds(v, conn.id, state, conn.turn, d)) continue;
    // … 2 lines
    if (!canStopComfortably(v.driver, v.v, d)) continue;
    // … 1 lines
    w.claims.releaseConnector(v.id, conn.id, w.conflicts);
  // … 4 lines
  }
}

// src/sim/intersections/admission.ts:266-347 slice of src/sim/intersections/admission.ts#evaluate (shared): the feature's statements; "…" marks code left out
function evaluate(w: SimWorld, r: Request, banker: BankerPass): Verdict {
  if (r.conn.maxBodyClass < bodyClassOfArchetype(r.v.archetype)) return { ok: false, reason: 'conflict' };
  const alreadyDeclared = r.v.reservedConnectors[0] === r.conn.id;
  const chain = alreadyDeclared
    ? r.v.reservedConnectors.map((id) => w.connector(id)).filter((c): c is Connector => !!c)
    : compactReservationChain(w, r.v, r.conn);
  if (!chain) return { ok: false, reason: 'spillback' };
  if (alreadyDeclared && chain.length !== r.v.reservedConnectors.length) {
    return { ok: false, reason: 'spillback' };
  }
  // … 1 lines
  if (alreadyDeclared && chain.length > 0 && r.row === 'none') return KEEP_DECLARED;
  // … 1 lines
  const reservations = chain.map((connector) => ({
    connector,
    points: w.conflicts.refs(connector.id).map((ref) => ref.point),
  }));
  // … 1 lines
  const current = reservations[0];
  if (!current) return { ok: false, reason: 'spillback' };
  if (r.row === 'none') return { ok: true, reservations, reserveOnly: true };
  // … 6 lines
  if (zipperHolds(w, r)) {
    return { ok: false, reason: 'conflict', reservations };
  }
  // … 2 lines
  if (!w.claims.available(current.points, r.v.id, current.connector.id,
    bodyClassOfArchetype(r.v.archetype), w.conflicts, (claim) => holderState(w, claim),
    (point, claim) => mergeFollows(w, r, point, claim))) {
    return { ok: false, reason: 'conflict', reservations };
  }
  if (queuedBodyInZone(w, r.v, current.connector)) {
    return { ok: false, reason: 'conflict', reservations };
  }
  if (exitSwingBlocked(w, r.v, current.connector)) {
    return { ok: false, reason: 'conflict', reservations };
  }
  if (!convoyCanEnter(w, r, current.connector)) {
    return { ok: false, reason: 'conflict', reservations };
  }
  if (movementReservedByOther(w, r.v, r.conn)) {
    return { ok: false, reason: 'conflict', reservations };
  }
  // … 1 lines
  if (!hasDownstreamStorage(w, r.v, r.conn, r.row === 'yield' || r.row === 'stop')) {
    return { ok: false, reason: 'spillback', reservations };
  }
  if (cycleFull(w, r.conn.fromLane, r.conn.toLane, r.v.archetype.length + Math.max(JAM_GAP, r.v.driver.s0))) {
    return { ok: false, reason: 'spillback', reservations };
  }
  if (outranksForExit(r)) {
    return { ok: false, reason: 'yield', reservations };
  }
  // … 1 lines
  if (!bankerSafeAfterGrant(w, r.v, reservations, banker)) {
    return { ok: false, reason: 'conflict', reservations };
  }
  // … 17 lines
  return { ok: true, reservations };
}

// src/sim/intersections/admission.ts:350-361 slice of src/sim/intersections/admission.ts#zipperHolds (shared): the feature's statements; "…" marks code left out
function zipperHolds(w: SimWorld, r: Request): boolean {
  if (w.mergeTurn.get(r.conn.toLane) !== r.conn.fromLane) return false;
  // … 2 lines
  for (const other of pending?.get(r.conn.node) ?? []) {
    if (other.v.id === r.v.id || other.v.admittedConnector) continue;
    if (other.conn.toLane !== r.conn.toLane || other.conn.fromLane === r.conn.fromLane) continue;
    if (RANK[other.row] < RANK[r.row]) continue;
    if (other.d <= ZIP_READY || other.v.v <= CONVOY_ROLLING) return true;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:364-381 slice of src/sim/intersections/admission.ts#mergeFollows (shared): the feature's statements; "…" marks code left out
function mergeFollows(
// … 4 lines
): boolean {
  // … 2 lines
  if ((w.doc.node(r.conn.node)?.incident.length ?? 0) !== 2) return false;
  // … 2 lines
  if (!holder || !theirs || theirs.toLane !== r.conn.toLane) return false;
  // … 3 lines
  const mine = Math.max(0, r.d) + r.conn.length;
  return ahead + holder.archetype.length + Math.max(JAM_GAP, r.v.driver.s0) <= mine;
}

// src/sim/intersections/admission.ts:384-392 slice of src/sim/intersections/admission.ts#mergeRemaining (shared): the feature's statements; "…" marks code left out
export function mergeRemaining(w: SimWorld, v: Vehicle, conn: Connector): number | null {
  if (v.lanelet === conn.id) return conn.length - v.s;
  if (v.lanelet === conn.fromLane) {
    const lane = w.lanelet(conn.fromLane);
    return lane ? lane.length - v.s + conn.length : null;
  }
  if (v.lanelet === conn.toLane) return -v.s;
// … 1 lines
}

// src/sim/intersections/admission.ts:400-408 slice of src/sim/intersections/admission.ts#outranksForExit (shared): the feature's statements; "…" marks code left out
function outranksForExit(r: Request): boolean {
  // … 1 lines
  for (const other of pending?.get(r.conn.node) ?? []) {
    if (other.v.id === r.v.id || other.v.admittedConnector) continue;
    if (RANK[other.row] <= RANK[r.row]) continue;
    if (other.conn.toLane === r.conn.toLane) return true;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:411-443 slice of src/sim/intersections/admission.ts#compactReservationChain (shared): the feature's statements; "…" marks code left out
export function compactReservationChain(
  // … 2 lines
  first: Connector,
): Connector[] | null {
  // … 1 lines
  if (first.maxBodyClass < body) return null;
  const chain: Connector[] = [first];
  const seen = new Set([first.id]);
  let routeIndex = v.route.indexOf(first.id);
  if (routeIndex < 0) return null;
  // … 1 lines
  let connector = first;
  while (true) {
    const out = w.lanelet(connector.toLane);
    if (!out || out.kind !== 'link') return null;
    // … 1 lines
    if (out.length + COARSE_EPS >= v.archetype.length + stoppingBuffer) return chain;
    // … 1 lines
    const exits = w.graph.exitsOf(out.id).filter((id) => (w.connector(id)?.maxBodyClass ?? -1) >= body);
    const nextId = v.route[routeIndex + 2];
    if (nextId === undefined) {
      return exits.length === 0 ? chain : null;
    }
    const next = w.connector(nextId);
    if (!next || next.maxBodyClass < body || next.fromLane !== out.id || seen.has(next.id)) return null;
    // … 1 lines
    chain.push(next);
    seen.add(next.id);
    connector = next;
    routeIndex += 2;
  }
}

// src/sim/intersections/admission.ts:446-461 slice of src/sim/intersections/admission.ts#movementReservedByOther (shared): the feature's statements; "…" marks code left out
function movementReservedByOther(w: SimWorld, v: Vehicle, conn: Connector): boolean {
  // … 1 lines
  if (w.conflicts.refs(conn.id).length > 0) return false;
  // … 3 lines
  const admitted = admittedBy ? admittedBy.get(conn.id) ?? [] : w.vehicles.values();
  for (const other of admitted) if (other.id !== v.id && other.admittedConnector === conn.id) return true;
  // … 1 lines
  for (const id of w.runtime.get(conn.id)?.order ?? []) {
    const other = w.vehicles.get(id);
    if (!other || other.id === v.id || other.lanelet !== conn.id) continue;
    if (other.s < need) return true;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:465-476 slice of src/sim/intersections/admission.ts#exitSwingBlocked (shared): the feature's statements; "…" marks code left out
function exitSwingBlocked(w: SimWorld, v: Vehicle, conn: Connector): boolean {
  // … 1 lines
  if (conn.turn === 'through' || bodyClassOfArchetype(v.archetype) < HEAVY) return false;
  // … 1 lines
  for (const sibling of w.graph.siblingLanes(conn.toLane)) {
    for (const body of w.bodiesIn(sibling)) {
      if (body.vehicle.id === v.id) continue;
      if (body.s - body.vehicle.archetype.length < reach) return true;
    }
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:478-497 slice of src/sim/intersections/admission.ts#queuedBodyInZone (shared): the feature's statements; "…" marks code left out
function queuedBodyInZone(w: SimWorld, v: Vehicle, conn: Connector): boolean {
  // … 1 lines
  for (const ref of w.conflicts.refs(conn.id)) {
    const point = w.conflicts.points[ref.point];
    const other = w.connector(ref.other);
    if (!point || !other) continue;
    const head = w.laneHead(other.fromLane);
    const lane = w.lanelet(other.fromLane);
    if (!head || !lane || head.id === v.id) continue;
    // An admitted head is protected by its own claim instead.
    if (head.admittedConnector) continue;
    // Only the movement the head is actually about to take.
    if (nextConnector(w, head)?.id !== other.id) continue;
    const z = point.zone(other.id, bodyClassOfArchetype(head.archetype), mine);
    if (!z) continue;
    const centre = head.s - lane.length - head.archetype.length / 2;
    if (centre >= z.enter) return true;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:500-522 slice of src/sim/intersections/admission.ts#stopShortOfIntrusion (shared): the feature's statements; "…" marks code left out
function stopShortOfIntrusion(w: SimWorld, v: Vehicle, conn: Connector, d: number): number | null {
  // … 2 lines
  for (const ref of w.conflicts.refs(conn.id)) {
    const point = w.conflicts.points[ref.point];
    if (!point) continue;
    for (const claim of w.claims.holdersAt(ref.point)) {
      if (claim.vehicle === v.id || claim.connector === conn.id) continue;
      // Ahead in the same merge: followed, not stopped short of.
      if (mergeFollows(w, { v, conn, d }, point, claim)) continue;
      const holder = holderState(w, claim);
      if (!holder) continue;
      const z = point.zone(conn.id, mine, holder.cls);
      const theirs = point.zone(claim.connector, holder.cls, mine);
      if (!z || !theirs || holder.centre > theirs.exit) continue;
      const frontAtEntry = z.enter + v.archetype.length / 2;
      if (frontAtEntry >= 0) continue;
      const gap = Math.max(0, d + frontAtEntry);
      stop = stop === null ? gap : Math.min(stop, gap);
    }
  }
  return stop;
}

// src/sim/intersections/admission.ts:525-547 slice of src/sim/intersections/admission.ts#convoyCanEnter (shared): the feature's statements; "…" marks code left out
function convoyCanEnter(w: SimWorld, r: Request, conn: Connector): boolean {
  const points = w.conflicts.refs(conn.id).map((ref) => ref.point);
  if (!points.length) return true;
  // … 1 lines
  for (const point of points) {
    for (const claim of w.claims.holdersAt(point)) {
      // Holders on OTHER movements have already been judged by the claim table
      if (claim.vehicle === r.v.id || claim.connector !== conn.id) continue;
      const owner = w.veh(claim.vehicle);
      if (owner) owners.set(owner.id, owner);
    }
  }
  // … 5 lines
  return [...owners.values()].every((owner) => {
    // … 1 lines
    const clearing = owner.clearingConnectors.some((token) => token.connector === conn.id);
    return (lane?.id === conn.id || clearing) && owner.v > CONVOY_ROLLING;
  });
}

// src/sim/intersections/admission.ts:567-579 slice of src/sim/intersections/admission.ts#connectorResources (shared): the feature's statements; "…" marks code left out
function connectorResources(
  // … 1 lines
  connector: Connector,
): readonly ResourceKey[] {
  const refs = w.conflicts.refs(connector.id);
  if (refs.length === 0) return [movementToken(connector.id)];
  let known = CONNECTOR_RESOURCES.get(refs);
  if (!known) {
    known = refs.map((ref) => ref.point);
    CONNECTOR_RESOURCES.set(refs, known);
  }
  return known;
}

// src/sim/intersections/admission.ts:582-588 slice of src/sim/intersections/admission.ts#holdsAnything (shared): the feature's statements; "…" marks code left out
function holdsAnything(w: SimWorld, v: Vehicle): boolean {
  // … 4 lines
  return !!lane && lane.kind === 'connector' && w.conflicts.refs(lane.id).length === 0;
}

// src/sim/intersections/admission.ts:590-603 slice of src/sim/intersections/admission.ts#actualAllocation (shared): the feature's statements; "…" marks code left out
function actualAllocation(w: SimWorld, v: Vehicle): Set<ResourceKey> {
  // … 7 lines
  if (lane?.kind === 'connector') movements.add(lane.id);
// … 4 lines
}

// src/sim/intersections/admission.ts:669-671 slice of src/sim/intersections/admission.ts#admissionProbe (shared): the feature's statements; "…" marks code left out
export const admissionProbe: {
  banker: ((w: SimWorld, applicant: Vehicle, proposal: readonly Connector[], safe: boolean) => void) | null;
} = { banker: null };

// src/sim/intersections/admission.ts:808-822 slice of src/sim/intersections/admission.ts#holderState (shared): the feature's statements; "…" marks code left out
export function holderState(w: SimWorld, claim: Claim): HolderState | null {
  // … 5 lines
  if (v.lanelet === conn.id) return { cls, centre: v.s - half };
  if (v.lanelet === conn.fromLane) {
    const lane = w.lanelet(conn.fromLane);
    return lane ? { cls, centre: v.s - lane.length - half } : null;
  }
  const token = v.clearingConnectors.find((t) => t.connector === conn.id);
  if (token) return { cls, centre: conn.length + token.distanceBeyondExit - half };
// … 1 lines
}

// src/sim/intersections/admission.ts:825-874 slice of src/sim/intersections/admission.ts#rightOfWay (shared): the feature's statements; "…" marks code left out
export function rightOfWay(
  // … 3 lines
  conn: Connector,
// … 1 lines
): RowClass {
  // … 1 lines
  const junction = w.graph.junctions.get(node);
  // … 3 lines
  if (controller && junction?.signalised) {
    const state = signalStateFor(controller, conn.group);
    if (signalHolds(v, conn.id, state, conn.turn, distanceToStop)) return 'none';

    // A movement is protected when the current stage gives green to nothing that physically conflicts with it from another
    const stage = controller.plan.stages[controller.stageIndex];
    return state !== 'red' && stage?.protectedMovements.includes(conn.id) ? 'signalGreen' : 'yield';
  }
  // … 7 lines
  if (policy === 'priority') {
    // … 1 lines
    const rule = n ? legRule(w.doc, n, conn.inSegment) : 'yield';
    return rule === 'priority' ? 'priority' : rule;
  }
  // … 3 lines
  if (policy === 'none') {
    // … 1 lines
    const highwayHere = isHighwayClass(w.doc, conn.inSegment);
    const highwayElsewhere = (n?.incident ?? []).some((s) => s !== conn.inSegment && isHighwayClass(w.doc, s));
    if (highwayHere && !(n?.incident ?? []).every((s) => isHighwayClass(w.doc, s))) return 'priority';
    if (highwayElsewhere && !highwayHere) return 'yield';
  // … 1 lines
  }
  // … 2 lines
  if (junction && conn.inSegment !== conn.outSegment && continues(w, node, conn)) {
    if (throughRank(w, conn) >= mainRoadRank(w, junction)) return 'priority';
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:877-882 slice of src/sim/intersections/admission.ts#throughRank (shared): the feature's statements; "…" marks code left out
function throughRank(w: SimWorld, conn: Connector): number {
  const incoming = w.doc.segment(conn.inSegment);
  const outgoing = w.doc.segment(conn.outSegment);
  return Math.min(incoming?.section?.priority ?? incoming?.type ?? 0,
    outgoing?.section?.priority ?? outgoing?.type ?? 0);
}

// src/sim/intersections/admission.ts:885-887 slice of src/sim/intersections/admission.ts#continues (shared): the feature's statements; "…" marks code left out
function continues(w: SimWorld, node: NodeId, conn: Connector): boolean {
  return conn.turn === 'through' || conn.carried || (w.doc.node(node)?.incident.length ?? 0) === 2;
}

// src/sim/intersections/admission.ts:890-890 slice of src/sim/intersections/admission.ts#mainRoads (shared): the feature's statements; "…" marks code left out
const mainRoads = new WeakMap<JunctionTopology, number>();

// src/sim/intersections/admission.ts:892-903 slice of src/sim/intersections/admission.ts#mainRoadRank (shared): the feature's statements; "…" marks code left out
function mainRoadRank(w: SimWorld, junction: JunctionTopology): number {
  let rank = mainRoads.get(junction);
  if (rank !== undefined) return rank;
  // … 1 lines
  for (const id of junction.connectors) {
    const conn = w.connector(id);
    if (!conn || conn.inSegment === conn.outSegment || !continues(w, junction.node, conn)) continue;
    rank = Math.max(rank, throughRank(w, conn));
  }
  mainRoads.set(junction, rank);
  return rank;
}

// src/sim/intersections/admission.ts:906-946 slice of src/sim/intersections/admission.ts#hasAcceptableGap (shared): the feature's statements; "…" marks code left out
function hasAcceptableGap(w: SimWorld, r: Request): boolean {
  // … 3 lines
  const critical = Math.max(
    CRITICAL_GAP_FLOOR,
    CRITICAL_GAP[r.conn.turn] * r.v.driver.gapFactor - impatience,
  );
  // … 3 lines
  for (const ref of w.conflicts.refs(r.conn.id)) {
    const other = w.connector(ref.other);
    if (!other) continue;
    // Giving way only to one side (V5): to the right where nothing is signed, to the left
    if (side !== null && !comesFrom(w, r.conn, other, side)) continue;
    // A movement from the same approach is not a stream this one gives way to
    if (other.inSegment === r.conn.inSegment) continue;
    if (!movementIsActive(w, other)) continue;

    // Anything approaching on the conflicting movement's origin lane.
    const approach = w.laneHead(other.fromLane);
    if (!approach) continue;
    // A lane may fan out into several connectors.
    if (approach.id === r.v.id || nextConnector(w, approach)?.id !== other.id) continue;
    // Only a body that can actually reach this one is a reason to wait.
    if (!w.conflicts.points[ref.point]?.zone(r.conn.id, mine, bodyClassOfArchetype(approach.archetype))) {
      continue;
    }
    const lane = w.lanelet(other.fromLane);
    if (!lane) continue;

    // A vehicle sitting still at its own stop line is not arriving
    if (approach.v < 0.5 && !approach.admittedConnector && !protectedNow(w, other)) continue;

    const distance = lane.length - approach.s;
    const arrival = distance / Math.max(approach.v, 0.5);
    if (arrival < need + critical) return false;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:949-960 slice of src/sim/intersections/admission.ts#clearTime (shared): the feature's statements; "…" marks code left out
function clearTime(w: SimWorld, r: Request): number {
  const refs = w.conflicts.refs(r.conn.id);
  // … 2 lines
  for (const ref of refs) exit = Math.max(exit, ref.exit[cls] ?? -Infinity);
  const last = Number.isFinite(exit)
    ? Math.max(0, exit + r.v.archetype.length / 2)
    : r.conn.length;
  const lanelet = w.lanelet(r.conn.lanelet);
  const speed = Math.max(m(0.8), Math.min(r.v.v0, lanelet?.speedLimit ?? r.v.v0) * 0.6);
  return (r.d + last) / speed;
}

// src/sim/intersections/admission.ts:963-968 slice of src/sim/intersections/admission.ts#protectedNow (shared): the feature's statements; "…" marks code left out
function protectedNow(w: SimWorld, conn: Connector): boolean {
  const controller = w.controller(conn.node);
  if (!controller || !w.graph.junctions.get(conn.node)?.signalised) return false;
  if (signalStateFor(controller, conn.group) !== 'green') return false;
  return controller.plan.stages[controller.stageIndex]?.protectedMovements.includes(conn.id) ?? false;
}

// src/sim/intersections/admission.ts:971-976 slice of src/sim/intersections/admission.ts#movementIsActive (shared): the feature's statements; "…" marks code left out
function movementIsActive(w: SimWorld, other: Connector): boolean {
  const controller = w.controller(other.node);
  const junction = w.graph.junctions.get(other.node);
  if (!controller || !junction?.signalised) return true;
  return signalStateFor(controller, other.group) !== 'red';
}

// src/sim/intersections/admission.ts:986-991 slice of src/sim/intersections/admission.ts#crossingBusy (shared): the feature's statements; "…" marks code left out
function crossingBusy(w: SimWorld, conn: Connector): boolean {
  for (const leg of crossingLegs(w, conn.node)) {
    if (pedestrianInSpan(w, conn.id, leg.id)) return true;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:1017-1044 slice of src/sim/intersections/admission.ts#crossingReachedFirst (shared): the feature's statements; "…" marks code left out
function crossingReachedFirst(w: SimWorld, r: Request): boolean {
  // … 1 lines
  if ((w.doc.node(r.conn.node)?.incident.length ?? 0) < 3) return false;
  const lane = w.lanelet(r.conn.lanelet);
  if (!lane) return false;
  // … 1 lines
  for (const leg of crossingLegs(w, r.conn.node)) {
    const crossing = leg.id;
    const occupants = w.crossingStates.get(crossing)?.occupants;
    if (!occupants?.length) continue;
    const span = w.crossingSpans.span(r.conn.id, crossing);
    if (!span) continue;
    if (top === 0) top = Math.max(2, Math.min(lane.speedLimit, slowestBend(r.v, lane)));
    // `pedestrianAhead` stops a vehicle for anybody within `PED_REACH_TIME` of the stretch until its front is over it
    let front = CLEAR_MARGIN * travelTime(Math.max(0, r.d) + span.along, r.v.v, top, r.v.driver.a);
    // Nor before the vehicles already on this movement ahead of it
    front = Math.max(front, aheadOnMovement(w, r, span.along, top));
    for (const p of occupants) {
      // Somebody held still on the zebra is waiting for something - very often for this very car - and is not arriving.
      if (p.held) continue;
      const at = p.s;
      const ahead = p.forward ? span.s0 - PED_BODY - at : at - span.s1 - PED_BODY;
      if (ahead <= 0) continue;
      if (ahead / Math.max(p.v, PED_MIN_PACE) < front + PED_REACH_TIME) return true;
    }
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:1047-1066 slice of src/sim/intersections/admission.ts#aheadOnMovement (shared): the feature's statements; "…" marks code left out
function aheadOnMovement(w: SimWorld, r: Request, along: number, top: number): number {
  // … 1 lines
  const approach = w.lanelet(r.conn.fromLane);
  // … 4 lines
  for (const id of w.rt(r.conn.lanelet).order) {
    const o = w.veh(id);
    if (o) consider(o, along - o.s);
  }
  if (approach) {
    for (const id of w.rt(approach.id).order) {
      const o = w.veh(id);
      if (!o || o.admittedConnector !== r.conn.id || o.s <= r.v.s) continue;
      consider(o, approach.length - o.s + along);
    }
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:1069-1100 slice of src/sim/intersections/admission.ts#pedestrianHasPriority (shared): the feature's statements; "…" marks code left out
function pedestrianHasPriority(w: SimWorld, r: Request): boolean {
  // … 1 lines
  const controller = w.controller(r.conn.node);
  const signalised = !!controller && !!w.graph.junctions.get(r.conn.node)?.signalised;
  for (const { segment, id } of crossingLegs(w, r.conn.node)) {
    const state = w.crossingStates.get(id);
    if (!state || (state.waitingFrom === 0 && state.waitingTo === 0)) continue;
    // A car standing on the zebra itself is what keeps them at the kerb (nobody steps off with a body on it)
    const edge = w.sidewalks.edges.get(w.sidewalks.crossings.get(id) ?? '');
    if (edge && vehicleBodyIntersectsCrossing(w, r.v, edge)) continue;
    const span = w.crossingSpans.span(r.conn.id, id);
    if (span === null) continue;
    // Only people who would reach the vehicle's path soon after stepping off
    if (!signalised || !controller) {
      // No zebra painted here - a road carrying on at another width, a link too short for one - is no zebra
      if (w.net.crosswalkDistanceAt(segment, r.conn.node) <= 0) continue;
      // Waited its share giving way to people arriving at the kerb: its turn now (`PED_COURTESY`)
      if (r.v.waited > PED_COURTESY) continue;
      // Uncontrolled zebra: give way to those who would soon be in the path
      if (span) {
        const reach = PED_MIN_PACE * PED_REACH_TIME;
        const near = (state.waitingFrom > 0 && span.s0 < reach) || (state.waitingTo > 0 && state.length - span.s1 < reach);
        if (!near) continue;
      }
      return true;
    }
    // At a signal too, a car that has waited its share is owed its turn
    if (r.v.waited > PED_COURTESY) continue;
    if (pedestrianSignalState(controller, id, state.length) === 'walk') return true;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:1103-1110 slice of src/sim/intersections/admission.ts#nextConnector (shared): the feature's statements; "…" marks code left out
export function nextConnector(w: SimWorld, v: Vehicle): Connector | undefined {
  // … 1 lines
  if (next) {
    // … 1 lines
    if (c && c.maxBodyClass >= bodyClassOfArchetype(v.archetype) && c.fromLane === v.lanelet) return c;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:1113-1120 slice of src/sim/intersections/admission.ts#yieldSide (shared): the feature's statements; "…" marks code left out
export function yieldSide(w: SimWorld, conn: Connector): 'right' | 'left' | null {
  const n = w.doc.node(conn.node);
  if (!n) return null;
  if (n.control === 'mini') return 'left';
  if (n.control !== 'none') return null;
  if (n.incident.some((s) => isHighwayClass(w.doc, s))) return null;
// … 1 lines
}

// src/sim/intersections/admission.ts:1123-1130 slice of src/sim/intersections/admission.ts#comesFrom (shared): the feature's statements; "…" marks code left out
export function comesFrom(w: SimWorld, conn: Connector, other: Connector, side: 'right' | 'left'): boolean {
  const mine = arrivalDirection(w, conn.fromLane);
  const theirs = arrivalDirection(w, other.fromLane);
  if (!mine || !theirs) return true;
  // … 1 lines
  const cross = mine.x * -theirs.y - mine.y * -theirs.x;
  return side === 'right' ? cross < -1e-3 : cross > 1e-3;
}

// src/sim/intersections/admission.ts:1132-1139 slice of src/sim/intersections/admission.ts#arrivalDirection (shared): the feature's statements; "…" marks code left out
function arrivalDirection(w: SimWorld, lane: string): { x: number; y: number } | null {
  // … 1 lines
  if (!l || l.centre.length < 0.5) return null;
  const end = l.centre.sampleAt(l.centre.length).p;
  const before = l.centre.sampleAt(Math.max(0, l.centre.length - m(2.4))).p;
  const len = Math.hypot(end.x - before.x, end.y - before.y) || 1;
  return { x: (end.x - before.x) / len, y: (end.y - before.y) / len };
}
