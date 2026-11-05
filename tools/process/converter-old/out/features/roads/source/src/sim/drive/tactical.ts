// The code of src/sim/drive/tactical.ts that is the feature's (the rest of the file is shared).

// src/sim/drive/tactical.ts:15-23 slice of src/sim/drive/tactical.ts#TripPlan (shared): the feature's statements; "…" marks code left out
export interface TripPlan {
  // … 1 lines
  readonly route: LaneletId[];
  // … 1 lines
  readonly changeTo: LaneletId | null;
  // … 1 lines
  readonly intent: LaneletId | null;
// … 1 lines
}

// src/sim/drive/tactical.ts:26-36 slice of src/sim/drive/tactical.ts#connectorCost (shared): the feature's statements; "…" marks code left out
function connectorCost(w: SimWorld, connectorId: LaneletId, body: BodyClass, shape: string): number {
  const connector = w.connector(connectorId);
  const out = connector && w.lanelet(connector.toLane);
  if (!connector || connector.maxBodyClass < body || !out || out.kind !== 'link' || w.rt(out.id).ghost) return Infinity;
  // … 1 lines
  if (!w.graph.laneUsable(out.id, shape)) return Infinity;
  const density = w.rt(out.id).order.length / Math.max(1, out.length / m(4.8));
  const travel = out.length / Math.max(m(0.2), out.speedLimit) * (1 + density * 2.6);
  const turn = connector.turn === 'uturn' ? 4 : connector.turn === 'through' ? 0 : 0.35;
  return connector.length / Math.max(m(0.2), out.speedLimit * 0.65) + travel + turn;
}

// src/sim/drive/tactical.ts:39-140 slice of src/sim/drive/tactical.ts#planTrip (shared): the feature's statements; "…" marks code left out
export function planTrip(
  // … 1 lines
  start: LaneletId,
  // … 1 lines
  goal: LaneletId,
// … 6 lines
): TripPlan | null {
  const startLane = w.lanelet(start);
  if (!startLane || startLane.kind !== 'link' || !w.lanelet(goal) || w.rt(goal).ghost) return null;
  if (start === goal) return { route: [start], changeTo: null, intent: null, cost: 0 };
  // … 1 lines
  if (!w.laneReaches.mayReach(w, start, goal, body)) return null;
  // … 2 lines
  scratch.begin(w.graph.lanelets.size + w.graph.connectors.size);
  // … 1 lines
  const end = goalEnd(w, goal)!;
  // … 5 lines
  const startSlot = scratch.slot(start);
  const startState = startSlot * 2;
  if (!startChange) scratch.markPlainNoChange(startState);
  scratch.setBest(startState, 0, -1, -1);
  scratch.push(startState, 0, toGo(startSlot));
  // … 2 lines
  while (scratch.size) {
    // … 6 lines
    if (nodeLane === goal) {
      reached = state;
      reachedCost = nodeCost;
      break;
    }
    // … 1 lines
    if (!lane || lane.kind !== 'link') continue;
    // … 1 lines
    for (const id of w.graph.exitsOf(nodeLane)) {
      const step = connectorCost(w, id, body, shape);
      if (!Number.isFinite(step)) continue;
      const out = w.connector(id)!.toLane;
      relax(scratch, scratch.slot(out) * 2, nodeCost + step, toGo, state, scratch.slot(id));
    }
    // … 1 lines
    if (scratch.changed(state) || lane.laneIndex === undefined) continue;
    const room = nodeLane === start ? lane.length - startS : lane.length;
    // … 1 lines
    for (const sibling of w.graph.changeTargets(nodeLane, shape)) {
      const other = w.lanelet(sibling);
      if (!other || other.kind !== 'link' || other.laneIndex === undefined || w.rt(sibling).ghost) continue;
      const lanes = Math.abs(other.laneIndex - lane.laneIndex);
      if (lanes === 0 || room < lanes * ROOM_PER_LANE) continue;
      relax(scratch, scratch.slot(sibling) * 2 + 1, nodeCost + lanes * CHANGE_COST, toGo, state, -1);
    }
  }
  if (reached < 0) return null;
  // … 2 lines
  const lanes: LaneletId[] = [], connectors: (LaneletId | null)[] = [];
  for (let state = reached; state !== startState;) {
    if (scratch.best(state) === Infinity) return null;
    const via = scratch.via[state]!;
    lanes.push(scratch.ids[state >> 1]!);
    connectors.push(via < 0 ? null : scratch.ids[via]!);
    state = scratch.parent[state]!;
  }
  const last = lanes.length - 1;
  // … 1 lines
  if (last >= 0 && connectors[last] === null) {
    // Change lanes here first. The route is the trip as driven without it.
    const changeTo = lanes[last]!;
    const intent = last >= 1 ? connectors[last - 1]! : null;
    const without = fallback ? planTrip(w, start, startS, goal, body, false, shape) : null;
    return {
      route: without?.route ?? [start],
      changeTo,
      intent,
      cost: reachedCost,
    };
  }
  // … 1 lines
  const route: LaneletId[] = [start];
  for (let k = last; k >= 0; k--) {
    // The next change is made on the lane the route has reached: it ends there.
    const connector = connectors[k]!;
    if (connector === null) break;
    route.push(connector, lanes[k]!);
  }
  return { route, changeTo: null, intent: null, cost: reachedCost };
}
