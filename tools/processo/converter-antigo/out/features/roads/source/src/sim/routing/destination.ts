// The code of src/sim/routing/destination.ts that is the feature's (the rest of the file is shared).

// src/sim/routing/destination.ts:9-12 slice of src/sim/routing/destination.ts#reachableExits (shared): the feature's statements; "…" marks code left out
function reachableExits(w: SimWorld, start: LaneletId, body: BodyClass): readonly LaneletId[] {
  const exits = w.laneReaches.exitsFrom(w, start, body);
  return exits.includes(start) ? exits.filter((id) => id !== start) : exits;
}

// src/sim/routing/destination.ts:15-19 slice of src/sim/routing/destination.ts#chooseVehicleDestination (shared): the feature's statements; "…" marks code left out
export function chooseVehicleDestination(w: SimWorld, start: LaneletId, body: BodyClass): LaneletId | null {
  const exits = reachableExits(w, start, body);
  if (exits.length === 0) return null;
  return exits[Math.floor(w.rng.route.float() * exits.length)] ?? exits[0] ?? null;
}

// src/sim/routing/destination.ts:22-76 slice of src/sim/routing/destination.ts#routeToDestination (shared): the feature's statements; "…" marks code left out
export function routeToDestination(w: SimWorld, start: LaneletId, goal: LaneletId, body: BodyClass): LaneletId[] | null {
  if (start === goal) return [start];
  if (!w.lanelet(start) || !w.lanelet(goal) || w.rt(goal).ghost) return null;
  // … 1 lines
  if (!w.laneReaches.mayReach(w, start, goal, body)) return null;
  // … 1 lines
  scratch.begin(w.graph.lanelets.size + w.graph.connectors.size);
  // … 1 lines
  const end = goalEnd(w, goal)!;
  // … 6 lines
  const startSlot = scratch.slot(start);
  const startState = startSlot * 2;
  scratch.setBest(startState, 0, -1, -1);
  scratch.push(startState, 0, toGo(startSlot));
  // … 2 lines
  while (scratch.size) {
    // … 6 lines
    if (currentLane === goal) {
      // DO NOT `unshift`: the route is gathered goal first and turned round once.
      const route: LaneletId[] = [goal];
      for (let at = state; at !== startState;) {
        if (scratch.best(at) === Infinity) return null;
        const from = scratch.parent[at]!;
        route.push(scratch.ids[scratch.via[at]!]!, scratch.ids[from >> 1]!);
        at = from;
      }
      return route.reverse();
    }
    for (const id of w.graph.exitsOf(currentLane)) {
      const connector = w.connector(id);
      const out = connector && w.lanelet(connector.toLane);
      if (!connector || connector.maxBodyClass < body || !out || out.kind !== 'link' || w.rt(out.id).ghost) continue;
      const density = w.rt(out.id).order.length / Math.max(1, out.length / m(4.8));
      const travel = out.length / Math.max(m(0.2), out.speedLimit) * (1 + density * 2.6);
      const turn = connector.turn === 'uturn' ? 4 : connector.turn === 'through' ? 0 : 0.35;
      const cost = currentCost + connector.length / Math.max(m(0.2), out.speedLimit * 0.65) + travel + turn;
      const outSlot = scratch.slot(out.id);
      if (cost >= scratch.best(outSlot * 2) - 1e-9) continue;
      scratch.setBest(outSlot * 2, cost, state, scratch.slot(id));
      scratch.push(outSlot * 2, cost, cost + toGo(outSlot));
    }
  }
// … 1 lines
}
