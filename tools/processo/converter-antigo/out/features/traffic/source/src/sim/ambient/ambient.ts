// The code of src/sim/ambient/ambient.ts that is the feature's (the rest of the file is shared).

// src/sim/ambient/ambient.ts:46-48 slice of src/sim/ambient/ambient.ts#TRAFFIC (shared): the feature's statements; "…" marks code left out
export const TRAFFIC: readonly (readonly [Archetype, number])[] = ARCHETYPES
  .filter((a) => a.id !== 'bus')
  .map((a) => [a, a.id === 'truck' ? a.weight * 0.4 : a.weight] as const);

// src/sim/ambient/ambient.ts:49-49 slice of src/sim/ambient/ambient.ts#PARKED_KINDS (shared): the feature's statements; "…" marks code left out
const PARKED_KINDS = ARCHETYPES.filter((a) => a.id === 'hatch' || a.id === 'sedan' || a.id === 'suv' || a.id === 'van');

// src/sim/ambient/ambient.ts:52-60 slice of src/sim/ambient/ambient.ts#SceneryCar (shared): the feature's statements; "…" marks code left out
export interface SceneryCar extends StandingCar {
  free: FreePose;
  // … 5 lines
  readonly lanelet: LaneletId;
}

// src/sim/ambient/ambient.ts:68-68 slice of src/sim/ambient/ambient.ts#AmbientWorld.parked (shared): the feature's statements; "…" marks code left out
  readonly parked: StandingCar[] = [];

// src/sim/ambient/ambient.ts:70-70 slice of src/sim/ambient/ambient.ts#AmbientWorld.standing (shared): the feature's statements; "…" marks code left out
  private readonly standing = new Set<StandingCar>();

// src/sim/ambient/ambient.ts:72-72 slice of src/sim/ambient/ambient.ts#AmbientWorld.parkedAt (shared): the feature's statements; "…" marks code left out
  private readonly parkedAt = new Map<StandingCar, number>();

// src/sim/ambient/ambient.ts:74-74 slice of src/sim/ambient/ambient.ts#AmbientWorld.movers (shared): the feature's statements; "…" marks code left out
  readonly movers = new Set<Vehicle>();

// src/sim/ambient/ambient.ts:76-76 slice of src/sim/ambient/ambient.ts#AmbientWorld.parkedLog (shared): the feature's statements; "…" marks code left out
  private parkedLog: { readonly car: StandingCar; readonly added: boolean }[] = [];

// src/sim/ambient/ambient.ts:120-153 slice of src/sim/ambient/ambient.ts#AmbientWorld.reset (shared): the feature's statements; "…" marks code left out
  reset(w: SimWorld): void {
    for (const id of this.walkers) removeWalker(w, id);
    // … 19 lines
    w.city.lots.reset();
  // … 11 lines
  }

// src/sim/ambient/ambient.ts:155-195 slice of src/sim/ambient/ambient.ts#AmbientWorld.step (shared): the feature's statements; "…" marks code left out
  step(w: SimWorld): void {
    // … 2 lines
    for (const id of this.walkers) if (!walkerAlive(w, id)) this.walkers.delete(id);
    // … 27 lines
    const wantWalkers = Math.min(MAX_WALKERS, Math.round(stepsIn(road, 100) * density(PEOPLE)));
    this.people(w, focus, reach, wantWalkers);
    // … 1 lines
    if (!w.meso) {
      const carRoad = this.roadAround(w, focus.x, focus.y, carReach);
      const wantDrivers = Math.min(focus.detail ? MAX_DRIVERS : MAX_DRIVERS_FAR, Math.round(stepsIn(carRoad, 100) * Math.max(0.25, density(CARS))));
      this.traffic(w, focus, carReach, wantDrivers);
    }
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:183-185 slice of src/sim/ambient/ambient.ts#AmbientWorld.step.density (shared): the feature's statements; "…" marks code left out
    const density = (table: Mix): number =>
      table.home * mix.home * busy('home', hour) + table.shop * mix.shop * busy('shop', hour)
      + table.work * mix.work * busy('work', hour) + table.civic * mix.civic * busy('civic', hour);

// src/sim/ambient/ambient.ts:198-222 slice of src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings (shared): the feature's statements; "…" marks code left out
  private trimToCeilings(w: SimWorld): void {
    // … 1 lines
    let walkers = Math.min(TRIM_PER_LOOK, w.pedViews.length - Math.floor(PED_CEILING * w.populationShare));
    for (const id of this.walkers) {
      if (walkers <= 0) break;
      const at = walkerOf(w, id);
      if (!at || (focus && focus.detail && this.seen(w, focus, at.x, at.y, m(4)))) continue;
      removeWalker(w, id);
      // … 1 lines
      walkers--;
    }
    // … 2 lines
    let drivers = Math.min(TRIM_PER_LOOK, w.vehicles.size - Math.floor(FLEET_CEILING * w.populationShare));
    for (const id of this.drivers) {
      if (drivers <= 0) break;
      // … 2 lines
      if (!v || !pose || (focus && this.seen(w, focus, pose.p.x, pose.p.y, v.archetype.length))) continue;
      // … 3 lines
      drivers--;
    }
  }

// src/sim/ambient/ambient.ts:240-240 slice of src/sim/ambient/ambient.ts#AmbientWorld.walls (shared): the feature's statements; "…" marks code left out
  private readonly walls = new SightWalls();

// src/sim/ambient/ambient.ts:243-248 slice of src/sim/ambient/ambient.ts#AmbientWorld.behindWall (shared): the feature's statements; "…" marks code left out
  private behindWall(w: SimWorld, ex: number, ey: number, x: number, y: number, pad: number): boolean {
    this.walls.sync(w.buildings, w.doc.changes);
    // … 2 lines
    return this.walls.blocked(ex, ey, x + sx, y + sy) && this.walls.blocked(ex, ey, x - sx, y - sy);
  }

// src/sim/ambient/ambient.ts:250-289 slice of src/sim/ambient/ambient.ts#AmbientWorld.people (shared): the feature's statements; "…" marks code left out
  private people(w: SimWorld, focus: NonNullable<SimWorld['focus']>, reach: number, want: number): void {
    // … 3 lines
    for (const id of [...this.walkers]) {
      const at = walkerOf(w, id);
      if (!at) { this.walkers.delete(id); continue; }
      const d = Math.hypot(at.x - focus.x, at.y - focus.y);
      // … 1 lines
      const inSight = seen && this.seen(w, focus, at.x, at.y, m(4));
      if (!inSight && (d > far || this.walkers.size > want)) {
        removeWalker(w, id);
        this.walkers.delete(id);
      } else if (inSight) {
        // In sight on their last stretch: on to somewhere else, never ending where they are seen.
        const b = w.rng.ambient.float() * Math.PI * 2, e = reach * (0.3 + w.rng.ambient.float() * 0.7);
        walkOn(w, id, focus.x + Math.cos(b) * e, focus.y + Math.sin(b) * e, m(30));
      }
    }
  // … 20 lines
  }

// src/sim/ambient/ambient.ts:354-364 slice of src/sim/ambient/ambient.ts#AmbientWorld.peopleTarget (shared): the feature's statements; "…" marks code left out
  private peopleTarget(w: SimWorld): number {
    // … 5 lines
    const table = PEOPLE.home * mix.home * busy('home', hour) + PEOPLE.shop * mix.shop * busy('shop', hour)
      + PEOPLE.work * mix.work * busy('work', hour) + PEOPLE.civic * mix.civic * busy('civic', hour);
    const want = Math.round(stepsIn(street, 100) * table * w.pedestrianIntensity);
    return Math.min(Math.max(0, want), most, Math.floor(PED_CEILING * w.populationShare));
  }

// src/sim/ambient/ambient.ts:367-419 slice of src/sim/ambient/ambient.ts#AmbientWorld.edgePeople (shared): the feature's statements; "…" marks code left out
  private edgePeople(w: SimWorld): void {
    // … 11 lines
    if (this.gates && (this.placesReady(w) || !ends.length)) {
      // … 1 lines
      const frontage = this.places.frontages();
      PLACE_KINDS.forEach((kind, k) => {
        const people = PEOPLE[kind] * busy(kind, hour) * (frontage[k]! / m(100));
        const out = outShare(kind, hour);
        outW += people * out;
        inW += people * (1 - out);
      });
    }
    if (!ends.length && outW <= 0) return;
    // … 6 lines
    const local = ends.length ? Math.min(1, (outW + inW) / Math.max(1, want)) : 1;
    for (let made = 0, tries = 0; made < batch && tries < batch * 4; tries++) {
      // … 1 lines
      let fromLot: DoorWay | null = null;
      let toLot: DoorWay | null = null;
      if (outW + inW > 0 && w.rng.ambient.float() < local) {
        const out = !ends.length || w.rng.ambient.float() * (outW + inW) < outW;
        const first = this.pickPlace(w, hour, out, null);
        const second = first && w.rng.ambient.float() < local ? this.pickPlace(w, hour, !out, first) : null;
        if (out) { fromLot = first; toLot = second; } else { toLot = first; fromLot = second; }
      }
      // … 1 lines
      if (fromLot) {
        const door = fromLot.out[0]!;
        from = { node: -1, x: door.x, y: door.y };
      } else {
        if (!ends.length) continue;
        from = ends[Math.floor(w.rng.ambient.float() * ends.length)]!;
        // Not onto somebody standing there (out of a door, they wait inside until it is clear).
        if (engine.bridge.anyoneWithin(w, from.x, from.y, m(1.5), null)) continue;
      }
      if (this.walkFrom(w, from, fromLot, toLot) !== null) made++;
    }
  }

// src/sim/ambient/ambient.ts:422-457 slice of src/sim/ambient/ambient.ts#AmbientWorld.walkFrom (shared): the feature's statements; "…" marks code left out
  private walkFrom(w: SimWorld, from: { node: number; x: number; y: number }, fromLot: DoorWay | null, toLot: DoorWay | null): number | null {
    // … 4 lines
    if (toLot) {
      const door = toLot.out[0]!;
      to = { node: -1, x: door.x, y: door.y };
    } else {
      const end = this.pickEnd(w, from.node);
      if (end) to = end;
      else {
        const p = this.across(w, from);
        if (!p) return null;
        to = { node: -1, x: p.x, y: p.y };
        roams = true;
      }
    }
    // … 2 lines
    const head = fromLot ? fromLot.out : NO_WAY;
    const tail = toLot ? reversed(toLot) : NO_WAY;
    const trip = {
      trip: this.nextTrip++, fromX: from.x, fromY: from.y, toX: to.x, toY: to.y,
    // … 1 lines
    };
    const id = fromLot || toLot ? walkThrough(w, trip, head, tail) : engine.walkTrip.call(engine, w, trip);
    if (id === null) return null;
    this.walkers.add(id);
    if (roams) this.roaming.add(id);
    // … 1 lines
    if (fromLot) this.gateWalks.fromGate++;
    if (toLot) this.gateWalks.toGate++;
    this.company(w, id, ageClass, from, to, roams, head, tail);
    return id;
  }

// src/sim/ambient/ambient.ts:471-490 slice of src/sim/ambient/ambient.ts#AmbientWorld.keepWalking (shared): the feature's statements; "…" marks code left out
  private keepWalking(w: SimWorld): void {
    // … 2 lines
    for (const id of [...this.roaming]) {
      // … 1 lines
      if (!onLastStretch(w, id)) continue;
      const at = walkerOf(w, id);
      if (!at) continue;
      // … 1 lines
      if (lot && walkOn(w, id, lot.out[0]!.x, lot.out[0]!.y, m(30), reversed(lot))) {
        this.roaming.delete(id);
        this.gateWalks.toGate++;
        continue;
      }
      // … 1 lines
      if (end && walkOn(w, id, end.x, end.y, m(30))) { this.roaming.delete(id); continue; }
      const p = this.across(w, at);
      if (p) walkOn(w, id, p.x, p.y, m(30));
    }
  }

// src/sim/ambient/ambient.ts:504-512 slice of src/sim/ambient/ambient.ts#AmbientWorld.open (shared): the feature's statements; "…" marks code left out
  private open(w: SimWorld): void {
    // … 2 lines
    const cars = w.edgeTraffic && !w.meso ? trafficTarget(w) - w.vehicles.size : 0;
    // … 1 lines
    if (cars > 0) this.openCars(w, Math.min(cars, OPEN_CARS));
    // … 1 lines
    if (--this.openTicks <= 0 || (cars <= 0 && people <= 0)) this.endOpening();
  }

// src/sim/ambient/ambient.ts:520-533 slice of src/sim/ambient/ambient.ts#AmbientWorld.openCars (shared): the feature's statements; "…" marks code left out
  private openCars(w: SimWorld, n: number): void {
    // … 4 lines
    for (let made = 0, tries = 0; made < n && tries < n * 6; tries++) {
      // … 1 lines
      const pick = lanes[atRoll(at, rng.float() * total)]!;
      const arch = rng.weighted(TRAFFIC);
      if (pick.length < arch.length + m(3)) continue;
      const s = arch.length + m(0.4) + rng.float() * (pick.length - arch.length - m(0.8));
      if (spawnVehicleAt(w, pick.id, s, arch) !== null) made++;
    }
  }

// src/sim/ambient/ambient.ts:577-579 slice of src/sim/ambient/ambient.ts#AmbientWorld.refreshPlaces (shared): the feature's statements; "…" marks code left out
  private refreshPlaces(w: SimWorld): void {
    this.places.sync(w.buildings, w.doc.changes);
  }

// src/sim/ambient/ambient.ts:582-585 slice of src/sim/ambient/ambient.ts#AmbientWorld.placesReady (shared): the feature's statements; "…" marks code left out
  private placesReady(w: SimWorld): boolean {
    // … 1 lines
    return this.places.ready();
  }

// src/sim/ambient/ambient.ts:588-595 slice of src/sim/ambient/ambient.ts#AmbientWorld.pickPlace (shared): the feature's statements; "…" marks code left out
  private pickPlace(w: SimWorld, hour: number, out: boolean, not: DoorWay | null): DoorWay | null {
    // … 1 lines
    const coef = PLACE_KINDS.map((kind) => {
      const share = outShare(kind, hour);
      return PEOPLE[kind] * busy(kind, hour) * (out ? share : 1 - share);
    });
    return this.places.pick(w.rng.ambient, coef, not, (p) => joinsFootway(w, p), WALK_DECAY, m(40));
  }

// src/sim/ambient/ambient.ts:598-614 slice of src/sim/ambient/ambient.ts#AmbientWorld.company (shared): the feature's statements; "…" marks code left out
  private company(w: SimWorld, leader: number, age: PersonAgeClass, from: { x: number; y: number }, to: { x: number; y: number }, roams: boolean,
    head: readonly { x: number; y: number }[] = NO_WAY, tail: readonly { x: number; y: number }[] = NO_WAY): void {
    // … 10 lines
    const id = head.length || tail.length ? walkThrough(w, trip, head, tail) : engine.walkTrip.call(engine, w, trip);
    if (id === null) return;
    this.walkers.add(id);
    if (roams) this.roaming.add(id);
  }

// src/sim/ambient/ambient.ts:651-695 slice of src/sim/ambient/ambient.ts#AmbientWorld.traffic (shared): the feature's statements; "…" marks code left out
  private traffic(w: SimWorld, focus: NonNullable<SimWorld['focus']>, reach: number, want: number): void {
    // … 1 lines
    for (const id of [...this.drivers]) {
      // … 4 lines
      const inSight = pose !== null && this.seen(w, focus, pose.p.x, pose.p.y, v.archetype.length);
      if (inSight) {
        this.sighted.add(id);
        // Near the end of its way (a road off the map), somewhere else instead: a car in sight drives on.
        const at = v.route.indexOf(v.lanelet);
        if (at >= 0 && v.route.length - at <= 5) {
          const next = chooseVehicleDestination(w, v.lanelet, bodyClassOfArchetype(v.archetype));
          if (next && next !== v.destination) { v.destination = next; planFrom(w, v); }
        }
      } else this.sighted.delete(id);
      if (!inSight && (d > far || this.drivers.size > want)) {
        w.removeVehicle(v);
        this.drivers.delete(id);
        this.sighted.delete(id);
      }
    }
    // … 6 lines
    for (let tries = 0, made = 0; tries < 48 && made < MAKE_PER_LOOK * (anywhere ? 4 : 1) && this.drivers.size < want; tries++) {
      // … 5 lines
      const arch = w.rng.ambient.weighted(TRAFFIC);
      const s = arch.length + m(0.4) + w.rng.ambient.float() * Math.max(0, lane.length - arch.length - m(0.8));
      const at = lane.centre.sampleAt(s).p;
      const d = Math.hypot(at.x - focus.x, at.y - focus.y);
      const ok = anywhere ? d < reach : d < reach && !this.seen(w, focus, at.x, at.y, arch.length + m(4));
      if (!ok) continue;
      const id = spawnVehicleAt(w, lane.id, s, arch);
      if (id !== null) { this.drivers.add(id); made++; }
    }
  }

// src/sim/ambient/ambient.ts:698-706 slice of src/sim/ambient/ambient.ts#AmbientWorld.lanesAround (shared): the feature's statements; "…" marks code left out
  private lanesAround(w: SimWorld, focus: { x: number; y: number }, reach: number): { id: string; length: number; x: number; y: number }[] {
    // … 1 lines
    for (const lane of lanesInBox(w, focus.x - reach, focus.y - reach, focus.x + reach, focus.y + reach)) {
      if (lane.kind !== 'link' || lane.length < m(12)) continue;
      const mid = lane.centre.sampleAt(lane.length / 2).p;
      out.push({ id: lane.id, length: lane.length, x: mid.x, y: mid.y });
    }
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:709-712 slice of src/sim/ambient/ambient.ts#AmbientWorld.openingLanes (shared): the feature's statements; "…" marks code left out
  private openingLanes(w: SimWorld): { id: string; length: number; x: number; y: number }[] {
    if (w.topologyLog.state !== this.lanesFor) this.listLanes(w);
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:715-726 slice of src/sim/ambient/ambient.ts#AmbientWorld.listLanes (shared): the feature's statements; "…" marks code left out
  private listLanes(w: SimWorld): void {
    this.lanesFor = w.topologyLog.state;
    // … 1 lines
    for (const lane of w.graph.lanelets.values()) {
      if (lane.kind !== 'link' || lane.length < m(12)) continue;
      const mid = lane.centre.sampleAt(lane.length / 2).p;
      this.lanes.push({ id: lane.id, length: lane.length, x: mid.x, y: mid.y });
    }
  // … 3 lines
  }

// src/sim/ambient/ambient.ts:734-749 method AmbientWorld.release: the feature's own
  release(w: SimWorld, car: SceneryCar): Vehicle {
    this.leaveSource(car);
    this.vacated.add(car.spot);
    if (this.sceneryAt.get(car.spot) === car) {
      this.sceneryAt.delete(car.spot);
      this.sceneryWatch?.(car.spot);
    }
    this.removeParked(car);
    // Its driver drawn from the bay's seed after its look (`stand`: the share parked, the kind, the colour), as when it was stood.
    const r = new Rng(car.seed);
    r.float(); r.float(); r.float();
    const body = createVehicle(car.id, car.archetype, makeDriver(car.archetype, () => r.float()), car.color, car.lanelet, 0, w.clock.tick);
    body.seats = 0;
    body.free = { ...car.free };
    return body;
  }

// src/sim/ambient/ambient.ts:752-754 slice of src/sim/ambient/ambient.ts#AmbientWorld.isParked (shared): the feature's statements; "…" marks code left out
  isParked(body: StandingCar): boolean {
    return this.standing.has(body);
  }

// src/sim/ambient/ambient.ts:757-764 method AmbientWorld.addParked: the feature's own
  addParked(body: Vehicle): void {
    if (this.standing.has(body)) return;
    this.standing.add(body);
    this.parkedAt.set(body, this.parked.length);
    this.parked.push(body);
    this.movers.add(body);
    this.logParked(body, true);
  }

// src/sim/ambient/ambient.ts:767-776 slice of src/sim/ambient/ambient.ts#AmbientWorld.removeParked (shared): the feature's statements; "…" marks code left out
  removeParked(body: StandingCar): void {
    this.standing.delete(body);
    this.movers.delete(body as Vehicle);
    const at = this.parkedAt.get(body);
    if (at === undefined) return;
    // … 1 lines
    if (last !== body) { this.parked[at] = last; this.parkedAt.set(last, at); }
    this.parkedAt.delete(body);
    this.logParked(body, false);
  }

// src/sim/ambient/ambient.ts:784-788 slice of src/sim/ambient/ambient.ts#AmbientWorld.parkedSince (shared): the feature's statements; "…" marks code left out
  parkedSince(serial: number, reader?: object): readonly { readonly car: StandingCar; readonly added: boolean }[] | null {
  // … 3 lines
  }

// src/sim/ambient/ambient.ts:790-794 slice of src/sim/ambient/ambient.ts#AmbientWorld.logParked (shared): the feature's statements; "…" marks code left out
  private logParked(car: StandingCar, added: boolean): void {
    this.parkedLog.push({ car, added });
  // … 2 lines
  }

// src/sim/ambient/ambient.ts:821-823 method AmbientWorld.sceneryCarAt: the feature's own
  sceneryCarAt(spot: number): SceneryCar | undefined {
    return this.sceneryAt.get(spot);
  }

// src/sim/ambient/ambient.ts:830-849 slice of src/sim/ambient/ambient.ts#AmbientWorld.parking (shared): the feature's statements; "…" marks code left out
  private parking(w: SimWorld): void {
    const { serial, changed } = bayChangesSince(w, this.baysSerial, this);
    // … 1 lines
    const win = w.meso ? w.meso.microWindow() : null;
    const moved = !!w.meso && win !== this.parkedWindow;
    if (serial === this.baysSerial && changed !== null && !moved) return;
    this.baysSerial = serial;
    // … 2 lines
    if (w.meso) {
      this.parkInWindow(w, win, changed, movers);
      return;
    }
    if (changed === null) {
      for (const source of [...this.carsBySource.keys()]) this.standSource(w, source, false, movers);
      for (const source of baySources(w)) this.standSource(w, source, true, movers);
      return;
    }
    for (const source of new Set(changed)) this.standSource(w, source, true, movers);
  }

// src/sim/ambient/ambient.ts:852-873 slice of src/sim/ambient/ambient.ts#AmbientWorld.parkInWindow (shared): the feature's statements; "…" marks code left out
  private parkInWindow(w: SimWorld, win: { readonly x: number; readonly y: number; readonly r: number } | null, changed: readonly string[] | null,
    movers: ReadonlyMap<number, readonly Vehicle[]>): void {
    // … 1 lines
    const inside = new Set(win ? baySourcesIn(w, win.x - win.r, win.y - win.r, win.x + win.r, win.y + win.r) : []);
    // … 1 lines
    if (changed === null) {
      // … 1 lines
      for (const source of [...this.carsBySource.keys()]) this.standSource(w, source, false, movers);
    // … 1 lines
    }
    for (const source of [...stood]) {
      if (inside.has(source)) continue;
      this.standSource(w, source, false, movers);
    // … 1 lines
    }
    if (changed !== null) for (const source of new Set(changed)) if (stood.has(source)) this.standSource(w, source, true, movers);
    for (const source of inside) {
      if (stood.has(source)) continue;
      this.standSource(w, source, true, movers);
      stood.add(source);
    }
  }

// src/sim/ambient/ambient.ts:876-885 slice of src/sim/ambient/ambient.ts#AmbientWorld.moverCells (shared): the feature's statements; "…" marks code left out
  private moverCells(): Map<number, Vehicle[]> {
    const cells = new Map<number, Vehicle[]>();
    for (const car of this.movers) {
      if (!car.free) continue;
      const key = moverCell(Math.floor(car.free.x / MOVER_CELL), Math.floor(car.free.y / MOVER_CELL));
      const list = cells.get(key);
      if (list) list.push(car); else cells.set(key, [car]);
    }
    return cells;
  }

// src/sim/ambient/ambient.ts:888-903 slice of src/sim/ambient/ambient.ts#AmbientWorld.standSource (shared): the feature's statements; "…" marks code left out
  private standSource(w: SimWorld, source: string, fill: boolean, movers: ReadonlyMap<number, readonly Vehicle[]>): void {
    // … 7 lines
    for (const bay of baysOfSource(w, source)) {
      const car = this.stand(w, bay, was.get(spotKey(bay.x, bay.y)), movers);
      if (!car) continue;
      cars.push(car);
      this.sourceOf.set(car, source);
    }
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:906-943 slice of src/sim/ambient/ambient.ts#AmbientWorld.stand (shared): the feature's statements; "…" marks code left out
  private stand(w: SimWorld, bay: Bay, was: SceneryCar | undefined, movers: ReadonlyMap<number, readonly Vehicle[]>): SceneryCar | null {
    const spot = spotKey(bay.x, bay.y);
    // … 1 lines
    if (this.vacated.has(spot)) return null;
    const cx = Math.floor(bay.x / MOVER_CELL), cy = Math.floor(bay.y / MOVER_CELL);
    for (let i = cx - 1; i <= cx + 1; i++) {
      for (let j = cy - 1; j <= cy + 1; j++) {
        for (const car of movers.get(moverCell(i, j)) ?? []) if (Math.hypot(car.free!.x - bay.x, car.free!.y - bay.y) < MOVER_CELL) return null;
      }
    }
    // … 1 lines
    const seed = 0x9e3779b9 ^ Math.imul(Math.round(stepsIn(bay.x, 0.4) * 7) + Math.round(stepsIn(bay.y, 0.4) * 13) * 4099, 2654435761);
    const r = new Rng(seed);
    if (r.float() >= PARKED) return null;
    // … 1 lines
    const fits = PARKED_KINDS.filter((kind) => stallFits(bay, kind.length));
    const pick = r.float();
    if (!fits.length) return null;
    const arch = fits[Math.floor(pick * fits.length)]!;
    const colour = arch.palette[Math.floor(r.float() * arch.palette.length)]!;
    // … 1 lines
    const angle = Math.atan2(-bay.oy, -bay.ox);
    const free: FreePose = { x: bay.x, y: bay.y, angle, px: bay.x, py: bay.y, pangle: angle, lot: bay.building };
    // … 1 lines
    if (was && was.free.x === bay.x && was.free.y === bay.y) {
      car = was;
      car.free = free;
    } else {
      car = { id: w.nextVehicleId++, archetype: arch, color: colour, free, spot, seed, lanelet: bay.lane?.lanelet ?? '' };
    }
    this.sceneryAt.set(spot, car);
    this.sceneryWatch?.(spot);
    this.standing.add(car);
    this.parkedAt.set(car, this.parked.length);
    this.parked.push(car);
    this.logParked(car, true);
    return car;
  }

// src/sim/ambient/ambient.ts:946-953 slice of src/sim/ambient/ambient.ts#AmbientWorld.takeScenery (shared): the feature's statements; "…" marks code left out
  private takeScenery(car: SceneryCar): void {
    // … 1 lines
    if (this.sceneryAt.get(car.spot) === car) {
      // … 1 lines
      this.sceneryWatch?.(car.spot);
    }
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:969-969 slice of src/sim/ambient/ambient.ts#AmbientWorld.buildingMix (shared): the feature's statements; "…" marks code left out
  private readonly buildingMix = new BuildingMix();

// src/sim/ambient/ambient.ts:971-974 slice of src/sim/ambient/ambient.ts#AmbientWorld.mixAround (shared): the feature's statements; "…" marks code left out
  private mixAround(w: SimWorld, x: number, y: number, r: number): Mix {
    this.buildingMix.sync(w.buildings, w.doc.changes);
    return this.buildingMix.around(x, y, r);
  }

// src/sim/ambient/ambient.ts:977-977 slice of src/sim/ambient/ambient.ts#AmbientWorld.street (shared): the feature's statements; "…" marks code left out
  private readonly street = new StreetLength();

// src/sim/ambient/ambient.ts:979-981 slice of src/sim/ambient/ambient.ts#AmbientWorld.roadAround (shared): the feature's statements; "…" marks code left out
  private roadAround(w: SimWorld, x: number, y: number, r: number): number {
    return this.street.around(w.net, x, y, r);
  }

// src/sim/ambient/ambient.ts:987-987 slice of src/sim/ambient/ambient.ts#newPlaces (shared): the feature's statements; "…" marks code left out
const newPlaces = (): LotPlaces<DoorWay> => new LotPlaces<DoorWay>({ wayOf: doorWay });

// src/sim/ambient/ambient.ts:989-989 slice of src/sim/ambient/ambient.ts#REVERSED (shared): the feature's statements; "…" marks code left out
const REVERSED = new WeakMap<DoorWay, readonly { x: number; y: number }[]>();

// src/sim/ambient/ambient.ts:991-995 slice of src/sim/ambient/ambient.ts#reversed (shared): the feature's statements; "…" marks code left out
function reversed(way: DoorWay): readonly { x: number; y: number }[] {
  let back = REVERSED.get(way);
  if (!back) REVERSED.set(way, back = [...way.out].reverse());
  return back;
}
