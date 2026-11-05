// The code of src/sim/vehicles/laneChange.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/laneChange.ts:51-108 slice of src/sim/vehicles/laneChange.ts#decideLaneChanges (shared): the feature's statements; "…" marks code left out
function decideLaneChanges(w: SimWorld): void {
  for (const v of w.vehiclesInIdOrder()) {
    // … 3 lines
    if (!lane || lane.kind !== 'link') {
      v.laneIntent = null;
      continue;
    }
    // … 15 lines
    if (v.commute && v.commute.lanelet === lane.id && v.commute.at - v.s < COMMUTE_HOLD) {
      v.laneIntent = null;
      continue;
    }
    // … 2 lines
    let wanted: LaneletId | null = null;
    // … 1 lines
    if (target !== null && target !== lane.id) {
      wanted = mandatory(w, v, target);
      mandatoryMove = wanted !== null;
    } else if (target !== lane.id) {
      // Keep a discretionary intent alive while the move is still worth making: weighed (MOBIL) a few times a second, each driver on its own ticks (by its id); between, the intent stands while its lane is still beside and the room is there.
      wanted = (v.id + w.clock.tick) % MOBIL_EVERY === 0 ? discretionary(w, v, lane.id, lane.length, v.laneIntent) : standingIntent(w, v, lane.id, lane.length);
    }
    // … 1 lines
    if (wanted === null) {
      v.laneIntent = null;
      continue;
    }
    if (wanted !== v.laneIntent) {
      v.laneIntent = wanted;
      v.laneIntentSince = v.age;
    }
    // … 2 lines
    const lead = mandatoryMove && v.v < CRAWL * 2 ? SIGNAL_LEAD_CRAWLING : SIGNAL_LEAD;
    if (v.age - v.laneIntentSince < lead) continue;
    if (!gapIsSafe(w, v, wanted)) continue;
    v.laneChange = wanted;
  // … 2 lines
  }
}

// src/sim/vehicles/laneChange.ts:111-138 slice of src/sim/vehicles/laneChange.ts#mandatory (shared): the feature's statements; "…" marks code left out
function mandatory(w: SimWorld, v: Vehicle, target: LaneletId): LaneletId | null {
  // … 1 lines
  const to = w.lanelet(target);
  if (!lane || !to || to.kind !== 'link' || w.rt(target).ghost || to.length < v.s ||
      lane.segment !== to.segment || lane.from !== to.from || lane.to !== to.to ||
      lane.laneIndex === undefined || to.laneIndex === undefined) {
    v.desiredLane = null;
    v.movementIntent = null;
    return null;
  }
  // … 1 lines
  const steps = Math.abs(to.laneIndex - lane.laneIndex);
  const adjacent = laneletId(lane.segment!, lane.from!, lane.to!,
    lane.laneIndex + Math.sign(to.laneIndex - lane.laneIndex));
  // … 1 lines
  if (steps === 0 || !w.lanelet(adjacent) || w.rt(adjacent).ghost || !w.graph.changeTargets(lane.id, v.archetype.shape).includes(adjacent)) {
    v.desiredLane = null;
    v.movementIntent = null;
    return null;
  }
  // … 1 lines
  if (lane.length - v.s < roomNeeded(w, v, adjacent, steps, Math.min(v.v, CRAWL))) {
    v.desiredLane = null;
    v.movementIntent = null;
    return null;
  }
  return adjacent;
}

// src/sim/vehicles/laneChange.ts:141-143 slice of src/sim/vehicles/laneChange.ts#roomNeeded (shared): the feature's statements; "…" marks code left out
function roomNeeded(w: SimWorld, v: Vehicle, adjacent: LaneletId, steps: number, speed: number): number {
  return roomAt(v, sideways(w, v, adjacent), steps, speed);
}

// src/sim/vehicles/laneChange.ts:152-178 slice of src/sim/vehicles/laneChange.ts#positioningSpeedCap (shared): the feature's statements; "…" marks code left out
export function positioningSpeedCap(w: SimWorld, v: Vehicle): number {
  // … 4 lines
  if (!lane || !to || lane.kind !== 'link' || lane.laneIndex === undefined || to.laneIndex === undefined ||
      lane.segment !== to.segment) return Infinity;
  const steps = Math.abs(to.laneIndex - lane.laneIndex);
  if (steps === 0) return Infinity;
  const adjacent = laneletId(lane.segment!, lane.from!, lane.to!,
    lane.laneIndex + Math.sign(to.laneIndex - lane.laneIndex));
  // … 1 lines
  const left = lane.length - v.s - v.v;
  // … 1 lines
  const across = sideways(w, v, adjacent);
  if (roomAt(v, across, steps, Math.max(v.v, lane.speedLimit)) <= left) return Infinity;
  // … 2 lines
  let hi = Math.max(v.v, lane.speedLimit);
  for (let i = 0; i < 12; i++) {
    const mid = (lo + hi) / 2;
    if (roomAt(v, across, steps, mid) <= left) lo = mid;
    else hi = mid;
  }
  // … 1 lines
  return Math.max(lo, CRAWL);
}

// src/sim/vehicles/laneChange.ts:186-189 slice of src/sim/vehicles/laneChange.ts#changeLength (shared): the feature's statements; "…" marks code left out
function changeLength(w: SimWorld, v: Vehicle, target: LaneletId, speed = v.v): number {
  const across = sideways(w, v, target);
  return across === null ? 0 : laneChangeLength(across, speed, v.archetype.length);
}

// src/sim/vehicles/laneChange.ts:192-206 slice of src/sim/vehicles/laneChange.ts#sideways (shared): the feature's statements; "…" marks code left out
function sideways(w: SimWorld, v: Vehicle, target: LaneletId): number | null {
  // … 1 lines
  const other = w.lanelet(target);
  if (!lane || !other) return null;
  // … 1 lines
  const at = Math.min(Math.max(0, v.s), lane.length);
  const last = lastSideways;
  if (last.from === lane.centre && last.to === other.centre && last.at === at) return last.across;
  const across = other.centre.distanceTo(lane.centre.sampleAtInto(at, sidewaysFrame).p);
  last.from = lane.centre;
  last.to = other.centre;
  last.at = at;
  last.across = across;
  return across;
}

// src/sim/vehicles/laneChange.ts:215-219 slice of src/sim/vehicles/laneChange.ts#standingIntent (shared): the feature's statements; "…" marks code left out
function standingIntent(w: SimWorld, v: Vehicle, laneId: LaneletId, laneLength: number): LaneletId | null {
  // … 2 lines
  return w.graph.changeTargets(laneId, v.archetype.shape).includes(intent) && !w.rt(intent).ghost ? intent : null;
}

// src/sim/vehicles/laneChange.ts:222-312 slice of src/sim/vehicles/laneChange.ts#discretionary (shared): the feature's statements; "…" marks code left out
function discretionary(
  // … 2 lines
  laneId: LaneletId,
  // … 1 lines
  signalling: LaneletId | null,
): LaneletId | null {
  // … 3 lines
  const targets = w.graph.changeTargets(laneId, v.archetype.shape);
  if (!targets.length) return null;
  // … 3 lines
  const here = w.lanelet(laneId);
  const index = here?.laneIndex ?? 0;
  // … 3 lines
  const onward = w.graph.exitsOf(laneId).length > 0;
  const ahead = leaderIn(w, laneId, v.s, v.id);
  const behind = followerIn(w, laneId, v.s - v.archetype.length, v.id);
  const mine = idmAccel(v.driver, v.v, wanted, ahead ?? CLEAR_ROAD);
  // … 2 lines
  let best: LaneletId | null = null;
  // … 1 lines
  for (const candidate of targets) {
    const to = w.lanelet(candidate);
    if (!to || to.kind !== 'link' || w.rt(candidate).ghost) continue;
    if (Math.abs((to.laneIndex ?? 0) - index) !== 1) continue;
    if (to.length < v.s) continue;
    // Never move into a dead end — but only when the lane being left is not one itself.
    if (onward && w.graph.exitsOf(candidate).length === 0) continue;
    if (turnInto !== null && !w.graph.exitsOf(candidate)
      .some((id) => w.connector(id)?.outSegment === turnInto)) continue;

    const theirLeader = leaderIn(w, candidate, v.s, v.id);
    const theirFollower = followerIn(w, candidate, v.s - v.archetype.length, v.id);

    // Would the car behind me over there have to brake harder than a driver reasonably can?
    let follower = 0;
    if (theirFollower) {
      const f = theirFollower.vehicle;
      const before = idmAccel(
        f.driver,
        f.v,
        f.v0,
        leaderIn(w, candidate, f.s, f.id) ?? CLEAR_ROAD,
      );
      const after = idmAccel(f.driver, f.v, f.v0, {
        gap: Math.max(m(0.02), v.s - v.archetype.length - f.s),
        speed: v.v,
        kind: 'vehicle',
      });
      if (after < -f.driver.bEmergency * SAFE_BRAKE_SHARE) continue;
      follower = after - before;
    }

    // And the car behind me here, which I am about to stop blocking.
    let released = 0;
    if (behind) {
      const o = behind.vehicle;
      const before = idmAccel(o.driver, o.v, o.v0, {
        gap: Math.max(m(0.02), v.s - v.archetype.length - o.s),
        speed: v.v,
        kind: 'vehicle',
      });
      const after = idmAccel(o.driver, o.v, o.v0, ahead
        ? { gap: Math.max(m(0.02), ahead.gap + (v.s - o.s)), speed: ahead.speed, kind: 'vehicle' }
        : CLEAR_ROAD);
      released = after - before;
    }

    const theirs = idmAccel(v.driver, v.v, wanted, theirLeader ?? CLEAR_ROAD);
    const outward = (to.laneIndex ?? 0) > index;
    const bias = outward ? KEEP_SIDE_BIAS : -KEEP_SIDE_BIAS;
    const gain = theirs - mine + v.driver.politeness * (follower + released) + bias;

    // Once signalling, the driver carries on while it is still worth it at all; a fresh decision needs the full threshold.
    const threshold = candidate === signalling ? v.driver.laneThreshold * 0.35 : v.driver.laneThreshold;
    if (!(gain > threshold && gain > bestGain)) continue;
    // Deciding needs only a gap that could open; the move itself waits for the indicator and then checks the gap again
    if (candidate !== signalling && !gapIsSafe(w, v, candidate)) continue;
    bestGain = gain;
    best = candidate;
  }
  // … 1 lines
  return best;
}

// src/sim/vehicles/laneChange.ts:315-318 slice of src/sim/vehicles/laneChange.ts#nextOutSegment (shared): the feature's statements; "…" marks code left out
function nextOutSegment(w: SimWorld, v: Vehicle): number | null {
  // … 1 lines
  return next && next.fromLane === v.lanelet ? next.outSegment : null;
}

// src/sim/vehicles/laneChange.ts:328-349 slice of src/sim/vehicles/laneChange.ts#leaderIn (shared): the feature's statements; "…" marks code left out
function leaderIn(w: SimWorld, laneId: LaneletId, s: number, self: number): Neighbour | null {
  const order = w.rt(laneId).order;
  // … 1 lines
  let hi = order.length;
  while (lo < hi) {
    const mid = (lo + hi) >> 1;
    const at = w.veh(order[mid] as number);
    if ((at?.s ?? 0) <= s) lo = mid + 1;
    else hi = mid;
  }
  for (let i = lo; i < order.length; i++) {
    const other = w.veh(order[i] as number);
    if (!other || other.id === self) continue;
    return {
      vehicle: other,
      gap: Math.max(m(0.02), other.s - other.archetype.length - s),
      speed: other.v,
      kind: 'vehicle',
    };
  }
// … 1 lines
}

// src/sim/vehicles/laneChange.ts:352-373 slice of src/sim/vehicles/laneChange.ts#followerIn (shared): the feature's statements; "…" marks code left out
function followerIn(w: SimWorld, laneId: LaneletId, rear: number, self: number): Neighbour | null {
  const order = w.rt(laneId).order;
  // … 1 lines
  let hi = order.length;
  while (lo < hi) {
    const mid = (lo + hi) >> 1;
    const at = w.veh(order[mid] as number);
    if ((at?.s ?? 0) < rear) lo = mid + 1;
    else hi = mid;
  }
  for (let i = lo - 1; i >= 0; i--) {
    const other = w.veh(order[i] as number);
    if (!other || other.id === self) continue;
    return {
      vehicle: other,
      gap: Math.max(m(0.02), rear - other.s),
      speed: other.v,
      kind: 'vehicle',
    };
  }
// … 1 lines
}

// src/sim/vehicles/laneChange.ts:376-393 slice of src/sim/vehicles/laneChange.ts#canLeaveLane (shared): the feature's statements; "…" marks code left out
function canLeaveLane(w: SimWorld, v: Vehicle, target: LaneletId): boolean {
  // … 1 lines
  const other = w.lanelet(target);
  if (!lane || !other) return true;
  const start = sideways(w, v, target)!;
  const length = laneChangeLength(start, v.v, v.archetype.length);
  const clear = shadowClearDistance(v, start, length);
  const need = clear + Math.max(JAM_GAP, v.driver.s0);
  if (resting.ahead(w, v.lanelet, v.s, v.id) - v.s < need) return false;
  // … 1 lines
  for (const body of w.bodiesIn(v.lanelet)) {
    // … 3 lines
    if (gap >= -m(0.02) && gap < need && ahead.v < Math.max(v.v, CRAWL)) return false;
  }
// … 1 lines
}

// src/sim/vehicles/laneChange.ts:396-410 slice of src/sim/vehicles/laneChange.ts#restingRearAhead (shared): the feature's statements; "…" marks code left out
function restingRearAhead(w: SimWorld, laneId: LaneletId, s: number, self: number): number {
  const order = w.rt(laneId).order;
  // … 2 lines
  for (let i = order.length - 1; i >= 0; i--) {
    const ahead = w.veh(order[i] as number);
    if (!ahead || ahead.id === self) continue;
    if (ahead.s <= s) break;
    const own = ahead.v <= CRAWL ? ahead.s : ahead.s + restingReach(ahead);
    const nose = Math.max(ahead.s, Math.min(own, limit));
    rear = nose - ahead.archetype.length;
    limit = rear - Math.max(JAM_GAP, ahead.driver.s0);
  }
  return rear;
}

// src/sim/vehicles/laneChange.ts:420-420 slice of src/sim/vehicles/laneChange.ts#RestingRears.lanes (shared): the feature's statements; "…" marks code left out
  private readonly lanes = new Map<LaneletId, number>();

// src/sim/vehicles/laneChange.ts:435-454 slice of src/sim/vehicles/laneChange.ts#RestingRears.ahead (shared): the feature's statements; "…" marks code left out
  ahead(w: SimWorld, laneId: LaneletId, s: number, self: number): number {
    if (this.w !== w) return restingRearAhead(w, laneId, s, self);
    let at = this.lanes.get(laneId);
    if (at === undefined) this.lanes.set(laneId, at = this.build(w, laneId));
    if (at < 0) return restingRearAhead(w, laneId, s, self);
    const d = this.data, n = d[at]!, base = at + 1;
    // … 1 lines
    let lo = 0, hi = n;
    while (lo < hi) {
      const mid = (lo + hi) >> 1;
      if (d[base + mid * 3]! <= s) lo = mid + 1;
      else hi = mid;
    }
    // … 2 lines
    if (me && me.s > s) {
      for (let k = lo; k < n && d[base + k * 3]! <= me.s; k++) if (d[base + k * 3 + 2] === self) return restingRearAhead(w, laneId, s, self);
    }
    return lo < n ? d[base + lo * 3 + 1]! : Infinity;
  }

// src/sim/vehicles/laneChange.ts:456-489 slice of src/sim/vehicles/laneChange.ts#RestingRears.build (shared): the feature's statements; "…" marks code left out
  private build(w: SimWorld, laneId: LaneletId): number {
    const order = w.rt(laneId).order;
    // … 1 lines
    if (this.data.length < at + 1 + order.length * 3) {
      const more = new Float64Array(Math.max(this.data.length * 2, at + 1 + order.length * 3));
      more.set(this.data.subarray(0, at));
      this.data = more;
    }
    // … 2 lines
    for (let i = 0; i < order.length; i++) {
      const v = w.veh(order[i] as number);
      if (!v) continue;
      if (v.s < previous) sorted = false;
      previous = v.s;
      d[base + n * 3] = v.s;
      d[base + n * 3 + 2] = v.id;
      n++;
    }
    if (!sorted) return -1;
  // … 13 lines
  }

// src/sim/vehicles/laneChange.ts:505-520 slice of src/sim/vehicles/laneChange.ts#pullOutRoom (shared): the feature's statements; "…" marks code left out
export function pullOutRoom(w: SimWorld, v: Vehicle): number {
  // … 1 lines
  if (!lane || lane.kind !== 'link') return 0;
  // … 2 lines
  for (const sibling of w.graph.siblingLanes(lane.id)) {
    const other = w.lanelet(sibling);
    if (!other || Math.abs((other.laneIndex ?? 0) - (lane.laneIndex ?? 0)) !== 1) continue;
    here ??= lane.centre.sampleAtInto(Math.min(Math.max(0, v.s), lane.length), pullOutFrame).p;
    const start = other.centre.distanceTo(here);
    const length = laneChangeLength(start, 0, v.archetype.length);
    const room = shadowClearDistance(v, start, length) + Math.max(JAM_GAP, v.driver.s0);
    best = best === 0 ? room : Math.min(best, room);
  }
  return best;
}

// src/sim/vehicles/laneChange.ts:544-577 slice of src/sim/vehicles/laneChange.ts#gapIsSafe (shared): the feature's statements; "…" marks code left out
function gapIsSafe(w: SimWorld, v: Vehicle, target: LaneletId): boolean {
  if (!canLeaveLane(w, v, target)) return false;
  // … 2 lines
  const finish = v.s + changeLength(w, v, target) + FINISH_MARGIN;
  // … 1 lines
  const to = w.lanelet(target);
  if (to && to.kind === 'link' && finish > to.length) return false;
  // … 1 lines
  if (resting.ahead(w, target, v.s, v.id) < finish) return false;
  // … 2 lines
  for (const body of w.bodiesIn(target)) {
    const other = body.vehicle;
    if (other.id === v.id) continue;

    const otherFront = body.s;
    const otherRear = otherFront - other.archetype.length;
    if (otherFront <= rear) {
      const gap = rear - otherFront;
      const need = Math.max(JAM_GAP, other.driver.s0) + other.v * other.driver.T * 0.5;
      if (gap < need) return false;
    } else if (otherRear >= v.s) {
      const gap = otherRear - v.s;
      const need = Math.max(JAM_GAP, v.driver.s0) + v.v * v.driver.T * 0.5;
      if (gap < need) return false;
      if (otherRear < finish && other.v < Math.max(v.v, CRAWL)) return false;
    } else {
      // Overlapping our own body length: no room at all.
      return false;
    }
  }
// … 1 lines
}
