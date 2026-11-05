// The code of src/sim/vehicles/laneClosures.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/laneClosures.ts:6-13 slice of src/sim/vehicles/laneClosures.ts#Closure (shared): the feature's statements; "…" marks code left out
export interface Closure {
  readonly lanelet: LaneletId;
// … 5 lines
}

// src/sim/vehicles/laneClosures.ts:15-15 slice of src/sim/vehicles/laneClosures.ts#CLOSED (shared): the feature's statements; "…" marks code left out
const CLOSED = new WeakMap<SimWorld, Map<LaneletId, Closure[]>>();

// src/sim/vehicles/laneClosures.ts:36-38 slice of src/sim/vehicles/laneClosures.ts#closedOn (shared): the feature's statements; "…" marks code left out
export function closedOn(w: SimWorld, lanelet: LaneletId): readonly Closure[] {
  return CLOSED.get(w)?.get(lanelet) ?? NONE;
}

// src/sim/vehicles/laneClosures.ts:50-75 slice of src/sim/vehicles/laneClosures.ts#laneClear (shared): the feature's statements; "…" marks code left out
export function laneClear(w: SimWorld, lanelet: LaneletId, from: number, to: number, stopShort: number, margin = CLEAR_MARGIN): boolean {
  // … 1 lines
  for (const body of w.bodiesIn(lanelet)) {
    const rear = body.s - body.vehicle.archetype.length;
    if (rear < to + margin && body.s > from - stopShort - stopping(body.vehicle)) return false;
  }
  // … 1 lines
  const near = new Set<LaneletId>();
  for (const before of lanesInto(w, lanelet)) {
    near.add(before);
    for (const further of lanesInto(w, before)) near.add(further);
  }
  near.delete(lanelet);
  for (const id of near) {
    for (const vid of w.rt(id).order) {
      const v = w.veh(vid);
      if (!v || v.lanelet === lanelet) continue;
      const k = v.route.indexOf(lanelet);
      if (k <= 0 || k > 2) continue;
      let ahead = -v.s;
      for (let j = 0; j < k; j++) ahead += w.lanelet(v.route[j]!)?.length ?? 0;
      if (ahead + from - stopShort < stopping(v)) return false;
    }
  }
// … 1 lines
}

// src/sim/vehicles/laneClosures.ts:78-89 slice of src/sim/vehicles/laneClosures.ts#lanesInto (shared): the feature's statements; "…" marks code left out
function lanesInto(w: SimWorld, id: LaneletId): LaneletId[] {
  const lane = w.lanelet(id);
  if (!lane) return [];
  if (lane.kind === 'connector') return lane.fromLane === undefined ? [] : [lane.fromLane];
  if (lane.from === undefined) return [];
  const out: LaneletId[] = [];
  for (const cid of w.graph.junctions.get(lane.from)?.connectors ?? []) {
    const c = w.connector(cid);
    if (c && c.toLane === id) out.push(c.lanelet);
  }
  return out;
}
