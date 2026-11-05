// The code of src/sim/vehicles/spawn.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/spawn.ts:98-158 slice of src/sim/vehicles/spawn.ts#stepDispatch (shared): the feature's statements; "…" marks code left out
export function stepDispatch(w: SimWorld, enabled: boolean): void {
  // … 1 lines
  if (!w.edgeTraffic) return;
// … 57 lines
}

// src/sim/vehicles/spawn.ts:167-226 slice of src/sim/vehicles/spawn.ts#spawnAt (shared): the feature's statements; "…" marks code left out
function spawnAt(w: SimWorld, id: string): boolean {
  // … 24 lines
  for (const s of [0, (arch.length + entryClearance) / 2, arch.length + entryClearance]) {
    // … 1 lines
    if (w.pedEngine.bridge.anyoneWithin(w, f.p.x, f.p.y, arch.width / 2 + m(1), null)) return false;
  }
// … 30 lines
}

// src/sim/vehicles/spawn.ts:229-229 type DestinationPicker: the feature's own
export type DestinationPicker = (w: SimWorld, start: LaneletId, body: BodyClass) => LaneletId | null;

// src/sim/vehicles/spawn.ts:232-277 function spawnVehicleAt: the feature's own
export function spawnVehicleAt(w: SimWorld, id: string, s: number, arch: Archetype, pick: DestinationPicker = chooseVehicleDestination): number | null {
  const lane = w.lanelet(id);
  if (!lane || lane.kind !== 'link' || !w.graph.laneUsable(id, arch.shape)) return null;
  if (s < arch.length + m(0.4) || s > lane.length - m(0.4)) return null;
  const rear = s - arch.length;
  let leaderGap = Infinity;
  let leaderSpeed = Infinity;
  let follower: Vehicle | null = null;
  let followerGap = Infinity;
  for (const body of w.bodiesIn(id)) {
    const front = body.s, back = body.s - body.vehicle.archetype.length;
    if (back - JAM_GAP < s && front + JAM_GAP > rear) return null;
    if (back >= s && back - s < leaderGap) { leaderGap = back - s; leaderSpeed = body.vehicle.v; }
    if (front <= rear && rear - front < followerGap) { followerGap = rear - front; follower = body.vehicle; }
  }
  for (const at of [rear, (rear + s) / 2, s]) {
    const f = lane.centre.sampleAt(Math.min(lane.length, Math.max(0, at)));
    if (w.pedEngine.bridge.anyoneWithin(w, f.p.x, f.p.y, arch.width / 2 + m(1), null)) return null;
  }
  const driver = makeDriver(arch, () => w.rng.driver.float());
  const v0 = lane.speedLimit * arch.speedFactor * w.rng.driver.range(DRIVER_NOISE.lo, DRIVER_NOISE.hi) * (1 + driver.aggression * 0.07);
  const color = arch.palette[Math.floor(w.rng.spawnVehicles.float() * arch.palette.length)] as string;
  // Born at a cruise the car ahead allows
  const toEnd = Math.max(0, lane.length - s - driver.s0);
  const speed = Math.min(birthSpeed(driver, v0, v0 * w.rng.driver.range(0.6, 0.9), leaderGap, leaderSpeed),
    Math.sqrt(2 * driver.b * toEnd));
  // Nor in front of a car behind that could not keep its distance braking in comfort
  if (follower && idmAccel(follower.driver, follower.v, follower.v0,
    { gap: Math.max(m(0.004), followerGap), speed, kind: 'vehicle' as const }) < -follower.driver.b) return null;
  const vehicle = createVehicle(w.nextVehicleId++, arch, driver, color, id, v0, w.clock.tick);
  vehicle.s = s;
  vehicle.v = speed;
  // Nor faster than the bends ahead allow (SUMO inserts a vehicle only if it can brake for what is ahead, ibid.)
  vehicle.v = Math.min(vehicle.v, curveSpeedCap(w, vehicle));
  vehicle.prev = snapshot(vehicle);
  assignOccupancy(w, vehicle);
  w.vehicles.set(vehicle.id, vehicle);
  w.enterLanelet(vehicle, id);
  vehicle.destination = pick(w, id, bodyClassOfArchetype(vehicle.archetype));
  // Nowhere this body can go from here - every way on too tight for it - and it is not at a road's end: not inserted
  if (planFrom(w, vehicle) === null && w.graph.exitsOf(id).length > 0) {
    w.removeVehicle(vehicle);
    return null;
  }
  return vehicle.id;
}

// src/sim/vehicles/spawn.ts:295-325 slice of src/sim/vehicles/spawn.ts#stepDespawn (shared): the feature's statements; "…" marks code left out
export function stepDespawn(w: SimWorld): void {
  // … 11 lines
  for (const v of out) {
    // … 12 lines
    if (lane.kind === 'link' && atEnd && nowhereToGo && roadEnd && !w.ambient.sighted.has(v.id)) {
      if (v.destination === lane.id) w.completedTrips++;
      w.removeVehicle(v);
    }
  }
}
