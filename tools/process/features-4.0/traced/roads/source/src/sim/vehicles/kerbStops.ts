// The code of src/sim/vehicles/kerbStops.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/kerbStops.ts:22-48 slice of src/sim/vehicles/kerbStops.ts#KerbStop (shared): the feature's statements; "…" marks code left out
export interface KerbStop {
  // … 1 lines
  readonly lanelet: LaneletId;
// … 23 lines
}

// src/sim/vehicles/kerbStops.ts:303-362 slice of src/sim/vehicles/kerbStops.ts#plan (shared): the feature's statements; "…" marks code left out
function plan(w: SimWorld, v: Vehicle): void {
  // … 1 lines
  if (!lane || lane.kind !== 'link' || lane.segment === undefined || lane.laneIndex === undefined) return;
  // … 2 lines
  if (w.graph.siblingLanes(lane.id).some((id) => (w.lanelet(id)?.laneIndex ?? -1) > lane.laneIndex!)) return;
  const structure = w.doc.segment(lane.segment)?.structure ?? 'ground';
  if (structure !== 'ground') return;
  // … 2 lines
  const latest = lane.length - END_CLEARANCE;
  if (latest - earliest < m(10)) return;
  // … 1 lines
  if (v.errand === 'service') {
    planBusStop(w, v, lane.id, earliest, latest);
  // … 1 lines
  }
  if (v.errand === 'deliver') {
    const at = earliest + (latest - earliest) * 0.4;
    if (!footwayBeside(w, lane.id, at - doorAlongFromFront(v, 1))) return;
    if (queuedAhead(w, v, at)) return;
    // … 1 lines
    const out = newStop('drop', lane.id, at, 1, 1);
    out.keep = true;
    const back = newStop('pick', lane.id, at, 1, 1);
    back.hold = DELIVERY_TIME;
    v.kerbStop = out;
    v.kerbQueue = [back];
  // … 1 lines
  }
  // … 2 lines
  if (v.errand === 'drop') {
    // … 6 lines
    const at = earliest + (latest - earliest) * 0.35;
    if (!footwayBeside(w, lane.id, at - doorAlongFromFront(v, seat))) return;
    if (queuedAhead(w, v, at)) return;
    v.kerbStop = newStop('drop', lane.id, at, seat);
  // … 1 lines
  }
  // … 7 lines
  const person = w.pedEngine.bridge.hailable(w, lane.id, earliest, Math.min(latest, earliest + PICK_REACH));
  if (!person) return;
  const at = Math.min(latest, Math.max(earliest, person.s + doorAlongFromFront(v, seat)));
  if (queuedAhead(w, v, at)) return;
  const stop = newStop('pick', lane.id, at, seat);
  stop.pedId = person.id;
  v.kerbStop = stop;
}

// src/sim/vehicles/kerbStops.ts:365-397 slice of src/sim/vehicles/kerbStops.ts#planBusStop (shared): the feature's statements; "…" marks code left out
function planBusStop(w: SimWorld, v: Vehicle, lanelet: LaneletId, earliest: number, latest: number): void {
  // … 2 lines
  if (!footwayBeside(w, lanelet, at - doorAlongFromFront(v, 1))) return;
  // … 5 lines
  for (let i = 0; i < leaving; i++) {
    const task = newStop('drop', lanelet, at, occupied[occupied.length - 1 - i]!, 1);
    task.seatStage = false;
    tasks.push(task);
  }
  // … 3 lines
  for (let i = 0; i < 2 && i < free.length; i++) {
    // … 2 lines
    const walker = w.pedEngine.bridge.hailable(w, lanelet, doorAt - BUS_FETCH * 0.7, doorAt + BUS_FETCH * 0.7, taken);
    if (!walker) break;
    taken.add(walker.id);
    const task = newStop('pick', lanelet, at, free[i]!, 0);
    task.pedId = walker.id;
    task.seatStage = false;
    tasks.push(task);
  }
// … 4 lines
}

// src/sim/vehicles/kerbStops.ts:399-402 slice of src/sim/vehicles/kerbStops.ts#newStop (shared): the feature's statements; "…" marks code left out
function newStop(kind: TaskKind, lanelet: LaneletId, at: number, seat: number, door = seat): KerbStop {
  return { kind, lanelet, at, door, seat, phase: 'approach', t: 0, elapsed: 0,
    pedId: null, person: null, transferTime: 0, fetchTime: 0, walked: 0, keep: false, hold: 0, seatStage: true };
}

// src/sim/vehicles/kerbStops.ts:426-431 slice of src/sim/vehicles/kerbStops.ts#kerbSide (shared): the feature's statements; "…" marks code left out
function kerbSide(w: SimWorld, lanelet: LaneletId, s: number): { p: { x: number; y: number }; t: { x: number; y: number }; right: { x: number; y: number } } | null {
  const lane = w.lanelet(lanelet);
  if (!lane) return null;
  const f = lane.centre.sampleAt(Math.min(Math.max(0, s), lane.length));
  return { p: f.p, t: f.t, right: { x: f.t.y, y: -f.t.x } };
}

// src/sim/vehicles/kerbStops.ts:434-453 slice of src/sim/vehicles/kerbStops.ts#footwayBeside (shared): the feature's statements; "…" marks code left out
function footwayBeside(w: SimWorld, lanelet: LaneletId, s: number): { x: number; y: number; t: { x: number; y: number } } | null {
  const lane = w.lanelet(lanelet);
  const side = kerbSide(w, lanelet, s);
  if (!lane || !side) return null;
  // … 2 lines
  if (lane.segment === undefined) return null;
  // … 1 lines
  for (const edge of w.sidewalks.walksBeside(lane.segment)) {
    const hit = edge.path.closestPoint(side.p);
    const dx = hit.point.x - side.p.x;
    const dy = hit.point.y - side.p.y;
    if (dx * side.right.x + dy * side.right.y <= 0) continue;
    if (hit.distance < bestDistance) {
      bestDistance = hit.distance;
      best = { x: hit.point.x, y: hit.point.y, t: edge.path.sampleAt(hit.s).t };
    }
  }
  return best;
}

// src/sim/vehicles/kerbStops.ts:456-465 slice of src/sim/vehicles/kerbStops.ts#doorPlaces (shared): the feature's statements; "…" marks code left out
function doorPlaces(w: SimWorld, v: Vehicle, stop: KerbStop) {
  // … 2 lines
  const f = lane.centre.sampleAt(Math.min(Math.max(0, s), lane.length));
  const right = { x: f.t.y, y: -f.t.x };
  // … 1 lines
  const door = { x: f.p.x + right.x * (half + m(0.5)), y: f.p.y + right.y * (half + m(0.5)) };
  // … 1 lines
  return { door, foot, heading: Math.atan2(f.t.y, f.t.x) };
}

// src/sim/vehicles/kerbStops.ts:468-487 slice of src/sim/vehicles/kerbStops.ts#roomToOpen (shared): the feature's statements; "…" marks code left out
function roomToOpen(w: SimWorld, v: Vehicle, stop: KerbStop): boolean {
  // … 7 lines
  for (const lane of lanesInBox(w, door.x - reach, door.y - reach, door.x + reach, door.y + reach)) {
    for (const id of w.rt(lane.id).order) {
      if (id === v.id) continue;
      const other = w.veh(id);
      if (!other) continue;
      const half = other.archetype.length / 2;
      const p = lane.centre.sampleAt(Math.min(Math.max(0, other.s - half), lane.length)).p;
      if (Math.hypot(p.x - door.x, p.y - door.y) < DOOR_CLEAR + half) return false;
    }
  }
// … 1 lines
}
