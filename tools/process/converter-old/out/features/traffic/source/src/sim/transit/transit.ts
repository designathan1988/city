// The code of src/sim/transit/transit.ts that is the feature's (the rest of the file is shared).

// src/sim/transit/transit.ts:55-64 slice of src/sim/transit/transit.ts#Bus (shared): the feature's statements; "…" marks code left out
interface Bus {
  // … 1 lines
  id: VehicleId | null;
// … 6 lines
}

// src/sim/transit/transit.ts:107-107 slice of src/sim/transit/transit.ts#TransitSim.stopLanes (shared): the feature's statements; "…" marks code left out
  private stopLanes = new Map<number, BayLane>();

// src/sim/transit/transit.ts:117-153 slice of src/sim/transit/transit.ts#TransitSim.rebuild (shared): the feature's statements; "…" marks code left out
  private rebuild(w: SimWorld): void {
    // … 6 lines
    for (const s of t.stops) {
      // … 2 lines
      const lane = laneBeside(w, s.x, s.y, s.segment);
      if (lane) this.stopLanes.set(s.id, lane);
    }
  // … 23 lines
  }

// src/sim/transit/transit.ts:156-175 slice of src/sim/transit/transit.ts#TransitSim.placeBuses (shared): the feature's statements; "…" marks code left out
  private placeBuses(w: SimWorld, line: TransitLine, was: readonly Bus[], kept: Set<Bus>): void {
    {
      // … 4 lines
      for (let k = 0; k < line.vehicles; k++) {
        // … 7 lines
        if (v) v.commute = null;
      // … 2 lines
      }
    }
  }

// src/sim/transit/transit.ts:239-286 slice of src/sim/transit/transit.ts#TransitSim.followLanes (shared): the feature's statements; "…" marks code left out
  private followLanes(w: SimWorld): void {
    const { state, boxes } = laneChangesSince(w, this.lanesSeen);
    this.lanesSeen = state;
    if (boxes === null) { this.rebuild(w); return; }
    if (!boxes.length) return;
    // … 4 lines
    const reach = boxes.map((r): ChangeRect => [r[0] - LANE_BESIDE_REACH, r[1] - LANE_BESIDE_REACH, r[2] + LANE_BESIDE_REACH, r[3] + LANE_BESIDE_REACH]);
    for (const id of this.busStops.idsMeetingAny(reach)) {
      const s = this.stops.get(id);
      if (!s) continue;
      const lane = laneBeside(w, s.x, s.y, s.segment);
      const was = this.stopLanes.get(s.id);
      if (lane) this.stopLanes.set(s.id, lane); else this.stopLanes.delete(s.id);
      if (!sameLane(was, lane)) for (const line of this.busLinesAt.get(s.id) ?? NONE) lines.add(line);
    }
  // … 29 lines
  }

// src/sim/transit/transit.ts:289-298 slice of src/sim/transit/transit.ts#TransitSim.step (shared): the feature's statements; "…" marks code left out
  step(w: SimWorld): void {
    if (w.doc.transitRevision !== this.transitFor) {
      // … 2 lines
      this.lanesSeen = laneChangesSince(w, null).state;
    // … 1 lines
    } else this.followLanes(w);
  // … 2 lines
  }

// src/sim/transit/transit.ts:305-348 slice of src/sim/transit/transit.ts#TransitSim.stepBus (shared): the feature's statements; "…" marks code left out
  private stepBus(w: SimWorld, bus: Bus): void {
    // … 3 lines
    if (!v) {
      // … 9 lines
      if (v2) v2.seats |= 1;
    // … 2 lines
    }
    const c = v.commute;
    if (!c) { this.sendBus(w, line, bus, v); return; }
    // … 1 lines
    if (bus.dwell === 0 && !v.kerbStop && v.doors.every((open) => open === 0)) {
      const pose = vehiclePose(w, v, 1);
      const goal = this.stopPoint(w, line.stops[bus.next]!);
      if (pose && goal && !w.inFocusReach(pose.p.x, pose.p.y, AWAY_MARGIN)) {
        w.removeVehicle(v);
        bus.id = null;
        bus.wait = travelTime(Math.hypot(goal.x - pose.p.x, goal.y - pose.p.y), goal.speed);
        return;
      }
    }
    const at = v.lanelet === c.lanelet && v.v < STOPPED && v.s >= c.at - AT_STOP;
    if (!at) return;
  // … 10 lines
  }

// src/sim/transit/transit.ts:351-357 slice of src/sim/transit/transit.ts#TransitSim.stopPoint (shared): the feature's statements; "…" marks code left out
  private stopPoint(w: SimWorld, stopId: number): { x: number; y: number; speed: number } | null {
    // … 1 lines
    const l = lane ? w.lanelet(lane.lanelet) : undefined;
    if (!lane || !l) return null;
    const p = l.centre.sampleAt(Math.min(l.length, Math.max(0, lane.at))).p;
    return { x: p.x, y: p.y, speed: l.speedLimit };
  }

// src/sim/transit/transit.ts:369-385 slice of src/sim/transit/transit.ts#TransitSim.sendBus (shared): the feature's statements; "…" marks code left out
  private sendBus(w: SimWorld, line: TransitLine, bus: Bus, v: Vehicle): void {
    for (let k = 0; k < 2 * line.stops.length; k++) {
      // … 3 lines
      if (lane) {
        v.commute = { trip: -1, lanelet: lane.lanelet, at: lane.at + v.archetype.length / 2 };
        v.destination = lane.lanelet;
        planFrom(w, v);
        if (lane.lanelet === v.lanelet || v.destination === lane.lanelet) return;
        v.commute = null;
      }
    // … 3 lines
    }
  }

// src/sim/transit/transit.ts:387-409 slice of src/sim/transit/transit.ts#TransitSim.spawnBus (shared): the feature's statements; "…" marks code left out
  private spawnBus(w: SimWorld, line: TransitLine, bus: Bus): VehicleId | null {
    // … 1 lines
    const arch = ARCHETYPES.find((a) => a.id === 'bus');
    const l = lane ? w.lanelet(lane.lanelet) : undefined;
    if (!lane || !arch || !l) return null;
    // … 1 lines
    const start = Math.max(arch.length, lane.at - APPROACH);
    // … 1 lines
    for (const body of w.bodiesIn(l.id)) if (body.s > start - arch.length * 2 && body.s - body.vehicle.archetype.length < lane.at + arch.length) return null;
    const driver = makeDriver(arch, () => w.rng.driver.float());
    const v0 = l.speedLimit * arch.speedFactor * w.rng.driver.range(DRIVER_NOISE.lo, DRIVER_NOISE.hi);
    const v = createVehicle(w.nextVehicleId++, arch, driver, line.colour, l.id, v0, w.clock.tick);
    v.s = start;
    v.v = 0;
    v.prev = snapshot(v);
    v.errand = null;
    w.vehicles.set(v.id, v);
    w.enterLanelet(v, l.id);
    v.commute = { trip: -1, lanelet: lane.lanelet, at: lane.at + arch.length / 2 };
    v.destination = lane.lanelet;
    planFrom(w, v);
    return v.id;
  }

// src/sim/transit/transit.ts:464-470 method TransitSim.crossingNearEnd: the feature's own
  crossingNearEnd(lanelet: string, length: number): number | null {
    let out: number | null = null;
    for (const { crossing: c } of this.crossingsByLane.get(lanelet) ?? NO_CROSSINGS) {
      if (length - c.at < KEEP_CLEAR && (out === null || c.at < out)) out = c.at;
    }
    return out;
  }

// src/sim/transit/transit.ts:479-490 method TransitSim.closedOn: the feature's own
  closedOn(lanelet: string): number[] {
    let out: number[] | null = null;
    for (const { rail, crossing: c } of this.crossingsByLane.get(lanelet) ?? NO_CROSSINGS) {
      const n = rail.line.mode === 'metro' ? METRO_CARS : TRAIN_CARS;
      for (const train of rail.trains) {
        // Ahead of the train, coming up; or under it, its cars still across.
        const ahead = (c.loopS - train.s + rail.length) % rail.length;
        if (ahead < CROSSING_WARN || ahead > rail.length - n * CAR_LENGTH - m(10)) { (out ??= []).push(c.at); break; }
      }
    }
    return out ?? NONE;
  }

// src/sim/transit/transit.ts:493-531 method TransitSim.trainZones: the feature's own
  trainZones(ahead = true, near?: readonly [number, number, number, number]): { x: number; y: number; r: number }[][] {
    const zones = new Map<string, { x: number; y: number; r: number }[]>();
    const zone = (key: string): { x: number; y: number; r: number }[] => { let z = zones.get(key); if (!z) { z = []; zones.set(key, z); } return z; };
    // DO NOT make the discs of every train of the map for the walkers of a corner of it: a train whose cars and track ahead cannot reach `near` (the walkers' box) is left out.
    const far = (rail: RailLine, train: Train): boolean => {
      if (!near) return false;
      const head = this.pointAt(rail, train.s), reach = TRAIN_CARS * CAR_LENGTH + Math.max(TRAIN_AHEAD, train.v * AHEAD_TIME) + m(4);
      return head.x + reach < near[0] || head.x - reach > near[2] || head.y + reach < near[1] || head.y - reach > near[3];
    };
    for (const rail of ahead ? this.rails : []) {
      if (rail.line.mode === 'metro') continue;
      for (const train of rail.trains) {
        if (far(rail, train)) continue;
        // Standing at a station, it is ground to keep off only once it is about to leave.
        if (train.v < m(1) && train.dwell > AHEAD_TIME) continue;
        // Ahead by time, not distance: as long as it takes to walk across the track.
        const reach = Math.max(TRAIN_AHEAD, train.v * AHEAD_TIME);
        for (let d = m(4); d < reach; d += m(3)) {
          const p = this.pointAt(rail, train.s + d);
          zone(train.key).push({ x: p.x, y: p.y, r: m(1.9) });
        }
      }
    }
    // DO NOT make every train's cars (`trains`) to throw the metro's away: the trains at grade only.
    for (const rail of this.rails) {
      if (rail.line.mode === 'metro') continue;
      for (const train of rail.trains) {
        if (far(rail, train)) continue;
        // Discs close enough to overlap: no gap a person could slip through.
        for (let k = 0; k < TRAIN_CARS; k++) {
          const car = this.pointAt(rail, train.s - CAR_LENGTH * (k + 0.5));
          for (let d = -CAR_LENGTH / 2 + m(1.5); d <= CAR_LENGTH / 2 - m(1.5) + 1e-6; d += m(2.5)) {
            zone(train.key).push({ x: car.x + Math.cos(car.heading) * d, y: car.y + Math.sin(car.heading) * d, r: m(1.8) });
          }
        }
      }
    }
    return [...zones.values()];
  }

// src/sim/transit/transit.ts:560-580 slice of src/sim/transit/transit.ts#levelCrossings (shared): the feature's statements; "…" marks code left out
export function levelCrossings(w: SimWorld, points: readonly Vec2[], at: readonly number[]): LevelCrossing[] {
  // … 1 lines
  for (let i = 0; i < points.length; i++) {
    // … 3 lines
    for (const lane of lanesInBox(w, x0, y0, x1, y1)) {
      const c = lane.centre;
      for (let j = 1; j < c.n; j++) {
        const p = c.point(j - 1), q = c.point(j);
        if (Math.max(p.x, q.x) < x0 || Math.min(p.x, q.x) > x1 || Math.max(p.y, q.y) < y0 || Math.min(p.y, q.y) > y1) continue;
        const hit = crossAt(a, b, p, q);
        if (!hit) continue;
        const laneS = c.closestPoint({ x: p.x + (q.x - p.x) * hit.v, y: p.y + (q.y - p.y) * hit.v }).s;
        found.push({ lane: lane.id, j, i, crossing: { lanelet: lane.id, at: laneS, loopS: at[i]! + Math.hypot(b.x - a.x, b.y - a.y) * hit.u } });
      }
    }
  }
// … 2 lines
}
