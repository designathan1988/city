// The code of src/sim/intersections/spillback.ts that is the feature's (the rest of the file is shared).

// src/sim/intersections/spillback.ts:9-62 slice of src/sim/intersections/spillback.ts#hasDownstreamStorage (shared): the feature's statements; "…" marks code left out
export function hasDownstreamStorage(w: SimWorld, v: Vehicle, conn: Connector, strict = false): boolean {
  const rt = w.rt(conn.toLane);
  // … 4 lines
  for (const id of w.runtime.get(conn.fromLane)?.order ?? []) {
    const other = w.veh(id);
    if (other && other.id !== v.id && other.admittedConnector === conn.id) committed.push(other);
  }
  for (const id of w.runtime.get(conn.id)?.order ?? []) {
    const other = w.veh(id);
    if (other && other.id !== v.id && other.lanelet === conn.id) committed.push(other);
  }
  // … 3 lines
  const out = w.lanelet(conn.toLane);
  // … 2 lines
  if (out && out.length < v.archetype.length + Math.max(JAM_GAP, v.driver.s0) &&
      (rt.order.length > 0 || committed.length > 0)) return false;
  // … 2 lines
  if (rt.order.length === 0) {
    if (!committed.length || !out || (!strict && !endsAtSignal(w, out.to))) return true;
    let room = out.length;
    for (const other of committed) room -= other.archetype.length + Math.max(JAM_GAP, other.driver.s0);
    return room >= v.archetype.length + Math.max(JAM_GAP, v.driver.s0);
  }
  // … 1 lines
  if (!out) return false;
  // … 2 lines
  const tailId = rt.order[0];
  const tail = tailId === undefined ? undefined : w.veh(tailId);
  let free = tail ? tail.s - tail.archetype.length : out.length;
  // … 1 lines
  const resting = restingRear(w, rt.order);
  if (endsAtSignal(w, out.to) || Number.isFinite(resting)) {
    // Towards a signal a rolling tail is not discharging: it is rolling up to whatever holds it, very often the red.
    free = Math.min(out.length, resting);
    for (const other of committed) {
      free -= other.archetype.length + Math.max(JAM_GAP, other.driver.s0);
    }
  } else if (tail && (strict || tail.v <= CONVOY_ROLLING)) {
    // A rolling tail is discharging and the existing convoy rules keep its followers moving.
    for (const other of committed) {
      free -= other.archetype.length + Math.max(JAM_GAP, other.driver.s0);
    }
  }
  // … 1 lines
  return free >= v.archetype.length + Math.max(JAM_GAP, v.driver.s0);
}

// src/sim/intersections/spillback.ts:90-93 slice of src/sim/intersections/spillback.ts#endsAtSignal (shared): the feature's statements; "…" marks code left out
function endsAtSignal(w: SimWorld, node: NodeId | undefined): boolean {
  // … 1 lines
  return w.graph.junctions.get(node)?.signalised ?? false;
}
