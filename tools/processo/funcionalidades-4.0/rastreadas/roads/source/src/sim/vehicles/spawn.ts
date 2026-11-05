// The code of src/sim/vehicles/spawn.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/spawn.ts:55-57 slice of src/sim/vehicles/spawn.ts#roadLength (shared): the feature's statements; "…" marks code left out
function roadLength(w: SimWorld): number {
  return w.roadLength.of(w.net);
}

// src/sim/vehicles/spawn.ts:60-83 slice of src/sim/vehicles/spawn.ts#entryLanes (shared): the feature's statements; "…" marks code left out
function entryLanes(w: SimWorld): readonly string[] {
  let known = entries.get(w);
  if (!known) entries.set(w, known = { seen: null, lanes: new Set(), sorted: null });
  if (known.seen !== w.topologyLog.state) {
    const changes = w.topologyLog.since(known.seen);
    known.seen = w.topologyLog.state;
    const look = (id: string): void => { if (isEntry(w, id)) known!.lanes.add(id); else known!.lanes.delete(id); };
    if (changes === null) {
      // @rebuild A world's first topology, or a lane graph built whole.
      known.lanes.clear();
      for (const id of w.graph.lanelets.keys()) look(id);
    } else {
      for (const change of changes) {
        for (const id of change.lanelets) look(id);
        for (const node of change.junctions) for (const id of w.graph.outbound.get(node) ?? []) look(id);
      }
    }
    known.sorted = null;
  }
  const lanes = known.sorted ??= [...known.lanes].sort();
  // … 1 lines
  for (const id of lanes) if (w.runtime.get(id)?.ghost) return lanes.filter((l) => !w.runtime.get(l)?.ghost);
  return lanes;
}

// src/sim/vehicles/spawn.ts:87-90 slice of src/sim/vehicles/spawn.ts#isEntry (shared): the feature's statements; "…" marks code left out
function isEntry(w: SimWorld, id: string): boolean {
  const lane = w.graph.lanelets.get(id);
  return !!lane && lane.kind === 'link' && lane.length >= m(12) && lane.from !== undefined && w.doc.mapEdge(lane.from);
}

// src/sim/vehicles/spawn.ts:167-226 slice of src/sim/vehicles/spawn.ts#spawnAt (shared): the feature's statements; "…" marks code left out
function spawnAt(w: SimWorld, id: string): boolean {
  // … 4 lines
  if (!w.graph.laneUsable(id, arch.shape)) return false;
  // … 1 lines
  if (lane.to !== undefined && w.doc.degree(lane.to) > 1 &&
      w.graph.exitsOf(id).every((exit) =>
        (w.connector(exit)?.maxBodyClass ?? -1) < bodyClassOfArchetype(arch))) return false;
  // … 12 lines
  if (lane.length <= arch.length + entryClearance) return false;
  // … 2 lines
  for (const s of [0, (arch.length + entryClearance) / 2, arch.length + entryClearance]) {
    const f = lane.centre.sampleAt(Math.min(lane.length, s));
    if (w.pedEngine.bridge.anyoneWithin(w, f.p.x, f.p.y, arch.width / 2 + m(1), null)) return false;
  }
  // … 3 lines
  const v0 =
    lane.speedLimit *
    arch.speedFactor *
    w.rng.driver.range(DRIVER_NOISE.lo, DRIVER_NOISE.hi) *
    // A pushy driver wants more than the limit and a timid one less, on top of the ordinary spread.
    (1 + driver.aggression * 0.07);
  // … 4 lines
  const vehicle = createVehicle(w.nextVehicleId++, arch, driver, color, id, v0, w.clock.tick);
  vehicle.s = arch.length + entryClearance;
  vehicle.v = birthSpeed(driver, v0, Math.min(v0 * 0.4, lane.speedLimit * 0.4),
    tailRear - vehicle.s, tailSpeed);
  vehicle.prev = snapshot(vehicle);
  // … 1 lines
  assignOccupancy(w, vehicle);
  // … 1 lines
  w.vehicles.set(vehicle.id, vehicle);
  w.enterLanelet(vehicle, id);
  vehicle.destination = chooseVehicleDestination(w, id, bodyClassOfArchetype(vehicle.archetype));
  // … 1 lines
  if (planFrom(w, vehicle) === null && w.graph.exitsOf(id).length > 0) {
    w.removeVehicle(vehicle);
    return false;
  }
// … 1 lines
}

// src/sim/vehicles/spawn.ts:229-229 slice of src/sim/vehicles/spawn.ts#DestinationPicker (shared): the feature's statements; "…" marks code left out
export type DestinationPicker = (w: SimWorld, start: LaneletId, body: BodyClass) => LaneletId | null;

// src/sim/vehicles/spawn.ts:232-277 slice of src/sim/vehicles/spawn.ts#spawnVehicleAt (shared): the feature's statements; "…" marks code left out
export function spawnVehicleAt(w: SimWorld, id: string, s: number, arch: Archetype, pick: DestinationPicker = chooseVehicleDestination): number | null {
  // … 1 lines
  if (!lane || lane.kind !== 'link' || !w.graph.laneUsable(id, arch.shape)) return null;
  if (s < arch.length + m(0.4) || s > lane.length - m(0.4)) return null;
  // … 11 lines
  for (const at of [rear, (rear + s) / 2, s]) {
    const f = lane.centre.sampleAt(Math.min(lane.length, Math.max(0, at)));
    if (w.pedEngine.bridge.anyoneWithin(w, f.p.x, f.p.y, arch.width / 2 + m(1), null)) return null;
  }
  // … 1 lines
  const v0 = lane.speedLimit * arch.speedFactor * w.rng.driver.range(DRIVER_NOISE.lo, DRIVER_NOISE.hi) * (1 + driver.aggression * 0.07);
  // … 2 lines
  const toEnd = Math.max(0, lane.length - s - driver.s0);
  const speed = Math.min(birthSpeed(driver, v0, v0 * w.rng.driver.range(0.6, 0.9), leaderGap, leaderSpeed),
    Math.sqrt(2 * driver.b * toEnd));
  // … 1 lines
  if (follower && idmAccel(follower.driver, follower.v, follower.v0,
    { gap: Math.max(m(0.004), followerGap), speed, kind: 'vehicle' as const }) < -follower.driver.b) return null;
  const vehicle = createVehicle(w.nextVehicleId++, arch, driver, color, id, v0, w.clock.tick);
  vehicle.s = s;
  vehicle.v = speed;
  // … 1 lines
  vehicle.v = Math.min(vehicle.v, curveSpeedCap(w, vehicle));
  vehicle.prev = snapshot(vehicle);
  assignOccupancy(w, vehicle);
  w.vehicles.set(vehicle.id, vehicle);
  w.enterLanelet(vehicle, id);
  vehicle.destination = pick(w, id, bodyClassOfArchetype(vehicle.archetype));
  // … 1 lines
  if (planFrom(w, vehicle) === null && w.graph.exitsOf(id).length > 0) {
    w.removeVehicle(vehicle);
    return null;
  }
  return vehicle.id;
}

// src/sim/vehicles/spawn.ts:295-325 slice of src/sim/vehicles/spawn.ts#stepDespawn (shared): the feature's statements; "…" marks code left out
export function stepDespawn(w: SimWorld): void {
  // … 2 lines
  for (const id of w.occupied) {
    // … 1 lines
    if (on && (on.kind !== 'link' || on.to === undefined || !w.doc.mapEdge(on.to))) continue;
  // … 4 lines
  }
  // … 1 lines
  for (const v of out) {
    // … 6 lines
    const atEnd = lane.length - v.s < v.driver.s0 + 2;
    // … 1 lines
    const nowhereToGo = w.graph.exitsOf(v.lanelet).every((id) =>
      (w.connector(id)?.maxBodyClass ?? -1) < body);
    const roadEnd = lane.to !== undefined && w.doc.mapEdge(lane.to);
    // … 1 lines
    if (lane.kind === 'link' && atEnd && nowhereToGo && roadEnd && !w.ambient.sighted.has(v.id)) {
      if (v.destination === lane.id) w.completedTrips++;
      w.removeVehicle(v);
    }
  }
}
