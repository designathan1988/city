// The code of src/sim/routing/router.ts that is the feature's (the rest of the file is shared).

// src/sim/routing/router.ts:19-19 slice of src/sim/routing/router.ts#TrapIndex.bodies (shared): the feature's statements; "…" marks code left out
  private readonly bodies = new Map<number, { readonly seeds: Set<LaneletId>; traps: ReadonlySet<LaneletId> | null }>();

// src/sim/routing/router.ts:22-27 slice of src/sim/routing/router.ts#TrapIndex.traps (shared): the feature's statements; "…" marks code left out
  traps(w: SimWorld, body: number): ReadonlySet<LaneletId> {
  // … 4 lines
  }

// src/sim/routing/router.ts:30-49 slice of src/sim/routing/router.ts#TrapIndex.follow (shared): the feature's statements; "…" marks code left out
  private follow(w: SimWorld): void {
    const changes = w.topologyLog.since(this.seen);
    if (changes !== null && !changes.length) return;
    this.seen = w.topologyLog.state;
    if (changes === null) { this.bodies.clear(); return; }
    // … 1 lines
    const lanes = new Set<LaneletId>();
    for (const change of changes) {
      for (const lane of change.lanelets) lanes.add(lane);
      for (const node of change.junctions) for (const lane of w.graph.inbound.get(node) ?? []) lanes.add(lane);
    }
    for (const [body, known] of this.bodies) {
      for (const lane of lanes) {
        rebuildWork.routing++;
        if (w.graph.lanelets.get(lane)?.kind === 'link' && isSeed(w, lane, body)) known.seeds.add(lane);
        else known.seeds.delete(lane);
      }
    // … 1 lines
    }
  }

// src/sim/routing/router.ts:53-60 slice of src/sim/routing/router.ts#seedsOf (shared): the feature's statements; "…" marks code left out
function seedsOf(w: SimWorld, body: number): Set<LaneletId> {
  const seeds = new Set<LaneletId>();
  for (const lane of w.graph.lanelets.values()) {
    rebuildWork.routing++;
    if (lane.kind === 'link' && isSeed(w, lane.id, body)) seeds.add(lane.id);
  }
  return seeds;
}

// src/sim/routing/router.ts:63-70 slice of src/sim/routing/router.ts#fittingExits (shared): the feature's statements; "…" marks code left out
function fittingExits(w: SimWorld, lane: LaneletId, body: number): number {
  // … 1 lines
  for (const cid of w.graph.exitsOf(lane)) {
    const c = w.connector(cid);
    if (c && c.maxBodyClass >= body) n++;
  }
// … 1 lines
}

// src/sim/routing/router.ts:73-74 slice of src/sim/routing/router.ts#isSeed (shared): the feature's statements; "…" marks code left out
const isSeed = (w: SimWorld, lane: LaneletId, body: number): boolean =>
  w.graph.exitsOf(lane).length > 0 && fittingExits(w, lane, body) === 0;

// src/sim/routing/router.ts:77-97 slice of src/sim/routing/router.ts#peel (shared): the feature's statements; "…" marks code left out
function peel(w: SimWorld, body: number, seeds: ReadonlySet<LaneletId>): ReadonlySet<LaneletId> {
  const traps = new Set<LaneletId>(seeds);
  const left = new Map<LaneletId, number>();
  const stack = [...seeds];
  while (stack.length) {
    const id = stack.pop()!;
    const from = w.graph.lanelets.get(id)?.from;
    const junction = from === undefined ? undefined : w.graph.junctions.get(from);
    // The movements into this lane: its junction's connectors that end on it.
    for (const cid of junction?.connectors ?? []) {
      rebuildWork.routing++;
      const c = w.connector(cid);
      if (!c || c.toLane !== id || c.maxBodyClass < body) continue;
      if (w.graph.lanelets.get(c.fromLane)?.kind !== 'link') continue;
      const d = (left.get(c.fromLane) ?? fittingExits(w, c.fromLane, body)) - 1;
      left.set(c.fromLane, d);
      if (d === 0 && !traps.has(c.fromLane)) { traps.add(c.fromLane); stack.push(c.fromLane); }
    }
  }
  return traps;
}

// src/sim/routing/router.ts:100-104 slice of src/sim/routing/router.ts#trapLanes (shared): the feature's statements; "…" marks code left out
export function trapLanes(w: SimWorld, body: number): ReadonlySet<LaneletId> {
// … 3 lines
}

// src/sim/routing/router.ts:107-186 slice of src/sim/routing/router.ts#planFrom (shared): the feature's statements; "…" marks code left out
export function planFrom(w: SimWorld, v: Vehicle): LaneletId | null {
  // … 2 lines
  if (v.destination && w.lanelet(v.lanelet)?.kind === 'link') {
    // The trip is planned over the whole carriageway, lane changes included
    const plan = planTrip(w, v.lanelet, v.s, v.destination, body, true, shape);
    if (plan) {
      v.desiredLane = plan.changeTo;
      if (plan.changeTo) v.movementIntent = plan.intent;
      if (plan.route.length >= 3) {
        v.route = plan.route;
        w.noteRoute(v);
        if (!plan.changeTo) v.movementIntent = plan.route[1]!;
        return plan.route[1]!;
      }
      if (plan.changeTo) {
        // No way on from this lane towards the goal
        const own = w.graph.exitsOf(v.lanelet).filter((id) => (w.connector(id)?.maxBodyClass ?? -1) >= body);
        const pick = own.length ? chooseExit(w, own, body, new Set([v.lanelet]), shape) : null;
        const conn = pick ? w.connector(pick) : undefined;
        if (!pick || !conn) return null;
        v.route = [v.lanelet, pick, conn.toLane];
        w.noteRoute(v);
        return pick;
      }
    } else {
      v.destination = null;
    }
  }
  if (v.destination) {
    // … 2 lines
    if (trip && first) {
      // … 2 lines
      v.movementIntent = first.id;
      // … 1 lines
      return first.id;
    }
  // … 2 lines
  }
  const own = w.graph.exitsOf(v.lanelet).filter((id) => (w.connector(id)?.maxBodyClass ?? -1) >= body &&
    w.graph.laneUsable(w.connector(id)!.toLane, shape));
  // … 2 lines
  const siblings = w.graph.changeTargets(v.lanelet, shape);
  const union = siblings.length
    ? [...own, ...siblings.flatMap((id: LaneletId) => w.graph.exitsOf(id))]
      .filter((id) => (w.connector(id)?.maxBodyClass ?? -1) >= body && w.graph.laneUsable(w.connector(id)!.toLane, shape))
    : own;
  // … 2 lines
  const wanted = existingIntent &&
      existingIntent.maxBodyClass >= body &&
      (existingIntent.fromLane === v.lanelet || siblings.includes(existingIntent.fromLane))
    ? existingIntent.id
    : union.length ? chooseExit(w, union, body, new Set([v.lanelet]), shape) : null;
  const wantedConnector = wanted === null ? null : w.connector(wanted);
  v.movementIntent = wantedConnector?.id ?? null;
  if (wantedConnector && wantedConnector.fromLane !== v.lanelet) {
    v.desiredLane = wantedConnector.fromLane;
  } else {
    v.desiredLane = null;
  }
  // … 2 lines
  if (!own.length) return null;
  const best = wantedConnector?.fromLane === v.lanelet
    ? wantedConnector.id : chooseExit(w, own, body, new Set([v.lanelet]), shape);
  if (!best) return null;
  // … 1 lines
  const conn = w.connector(best);
  if (!conn) return null;
  // … 1 lines
  v.route = [v.lanelet, best, conn.toLane];
  // … 2 lines
  return best;
}

// src/sim/routing/router.ts:189-235 slice of src/sim/routing/router.ts#extend (shared): the feature's statements; "…" marks code left out
export function extend(w: SimWorld, v: Vehicle): void {
  // … 2 lines
  if (v.destination) {
    // … 3 lines
    if (lane?.kind === 'link') {
      // A change due on the tail lane is planned when the car gets there: its route without the change is not wanted.
      const plan = planTrip(w, tail!, 0, v.destination, body, true, shape, false);
      if (plan) {
        if (!plan.changeTo) { v.route.push(...plan.route.slice(1)); w.noteRoute(v); }
        return;
      }
      v.destination = null;
    }
  }
  // … 9 lines
  while (v.route.length < HORIZON && guard++ < HORIZON) {
    // … 3 lines
    if (!lane || lane.kind !== 'link') break;
    // … 1 lines
    const exits = w.graph.exitsOf(tail);
    if (!exits.length) break;
    // … 1 lines
    const pick = chooseExit(w, exits, body, undefined, shape);
    if (!pick) break;
    const conn = w.connector(pick);
    if (!conn) break;
    // … 2 lines
    if (v.route.includes(conn.toLane)) break;
    // … 1 lines
    v.route.push(pick, conn.toLane);
  }
// … 1 lines
}

// src/sim/routing/router.ts:237-267 slice of src/sim/routing/router.ts#chooseExit (shared): the feature's statements; "…" marks code left out
function chooseExit(
  // … 3 lines
  visited = new Set<LaneletId>(),
// … 2 lines
): string | null {
  // … 2 lines
  for (const cid of exits) {
    // … 1 lines
    if (!conn || conn.maxBodyClass < body || !w.graph.laneUsable(conn.toLane, shape)) continue;
    const out = w.lanelet(conn.toLane);
    if (!out || w.rt(conn.toLane).ghost || visited.has(out.id)) continue;
    const cost = routeCost(w, cid, body, visited, LOOKAHEAD);
    if (Number.isFinite(cost)) scored.push({ id: cid, cost });
  }
// … 12 lines
}

// src/sim/routing/router.ts:270-309 slice of src/sim/routing/router.ts#routeCost (shared): the feature's statements; "…" marks code left out
function routeCost(
  // … 1 lines
  connectorId: LaneletId,
  // … 1 lines
  visited: Set<LaneletId>,
// … 1 lines
): number {
  const connector = w.connector(connectorId);
  if (!connector || connector.maxBodyClass < body) return Infinity;
  const out = w.lanelet(connector.toLane);
  if (!out || out.kind !== 'link' || visited.has(out.id) || w.rt(out.id).ghost) return Infinity;
  // … 1 lines
  if (trapLanes(w, body).has(out.id)) return Infinity;
  // … 1 lines
  const runtime = w.rt(out.id);
  const density = runtime.order.length / Math.max(1, out.length / m(4.8));
  const travel = out.length / Math.max(m(0.2), out.speedLimit);
  const connectorTravel = connector.length / Math.max(m(0.2), out.speedLimit * 0.65);
  const turnPenalty = connector.turn === 'uturn' ? 4 : connector.turn === 'through' ? 0 : 0.35;
  const here = connectorTravel + travel * (1 + density * 2.6) + turnPenalty;
  const next = w.graph.exitsOf(out.id);
  // … 1 lines
  if (!next.length) return here;
  // … 1 lines
  if (!next.some((id) => (w.connector(id)?.maxBodyClass ?? -1) >= body)) return Infinity;
  if (remaining <= 1) return here;
  // … 2 lines
  visited.add(out.id);
  // … 1 lines
  for (const nextConnector of next) {
    continuation = Math.min(
      continuation,
      routeCost(w, nextConnector, body, visited, remaining - 1),
    );
  }
  visited.delete(out.id);
  // … 1 lines
  return here + (Number.isFinite(continuation) ? continuation * 0.72 : 0);
}

// src/sim/routing/router.ts:312-327 slice of src/sim/routing/router.ts#reconsiderRoute (shared): the feature's statements; "…" marks code left out
export function reconsiderRoute(w: SimWorld, v: Vehicle): void {
  // … 8 lines
  if (!lane || lane.kind !== 'link') return;
  // … 1 lines
  if (lane.length - v.s < v.archetype.length + v.driver.s0) return;
// … 3 lines
}

// src/sim/routing/router.ts:330-374 slice of src/sim/routing/router.ts#repairRoute (shared): the feature's statements; "…" marks code left out
export function repairRoute(w: SimWorld, v: Vehicle): void {
  // … 9 lines
  if (current.kind === 'connector') {
    const connector = w.connector(current.id);
    const to = connector ? w.lanelet(connector.toLane) : undefined;
    v.route = to ? [current.id, to.id] : [current.id];
    w.noteRoute(v);
    extend(w, v);
    return;
  }
  // … 1 lines
  const valid: LaneletId[] = [];
  let previous: LaneletId | undefined;
  for (const id of v.route) {
    // … 2 lines
    if (previous !== undefined && !isLegalTransition(w, previous, id, body)) break;
    valid.push(id);
  // … 1 lines
  }
  // … 1 lines
  if (!valid.length || valid[0] !== v.lanelet) {
    v.route = [v.lanelet];
    w.noteRoute(v);
    planFrom(w, v);
    return;
  }
  // … 1 lines
  v.route = valid;
// … 7 lines
}

// src/sim/routing/router.ts:391-402 slice of src/sim/routing/router.ts#isLegalTransition (shared): the feature's statements; "…" marks code left out
function isLegalTransition(w: SimWorld, fromId: LaneletId, toId: LaneletId,
  body: ReturnType<typeof bodyClassOfArchetype>): boolean {
  const from = w.lanelet(fromId);
  const to = w.lanelet(toId);
  if (!from || !to) return false;
  if (from.kind === 'link') {
    const connector = w.connector(toId);
    return !!connector && connector.maxBodyClass >= body && connector.fromLane === fromId;
  }
  const connector = w.connector(fromId);
  return !!connector && connector.toLane === toId;
}
