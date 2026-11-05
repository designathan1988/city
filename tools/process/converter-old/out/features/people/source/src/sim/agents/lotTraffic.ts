// The code of src/sim/agents/lotTraffic.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lotTraffic.ts:49-49 slice of src/sim/agents/lotTraffic.ts#KINDS (shared): the feature's statements; "…" marks code left out
const KINDS: readonly (keyof Mix)[] = ['home', 'shop', 'work', 'civic'];

// src/sim/agents/lotTraffic.ts:121-121 slice of src/sim/agents/lotTraffic.ts#LotTraffic.ambient (shared): the feature's statements; "…" marks code left out
  private ambient: AmbientWorld | null = null;

// src/sim/agents/lotTraffic.ts:145-158 slice of src/sim/agents/lotTraffic.ts#LotTraffic.track (shared): the feature's statements; "…" marks code left out
  private track(id: VehicleId, bay: Bay, phase: Phase, body: Vehicle | null, go: boolean): void {
    const car: LotCar = {
      // … 1 lines
      order: this.nextOrder++, slot: this.waiting.push(-1), ripe: false, kind: GONE, bayAt: spotKey(bay.x, bay.y), bodyAt: null,
    };
  // … 8 lines
  }

// src/sim/agents/lotTraffic.ts:192-217 method LotTraffic.reset: the feature's own
  reset(w?: SimWorld): void {
    this.lorries.reset(w);
    this.cars.clear();
    this.nextOrder = 0;
    this.active.clear();
    this.time = 0;
    this.comingCount = 0;
    this.movingCount = 0;
    this.movingIn.clear();
    this.owed = 0;
    this.clock = 0;
    this.baysRead = -1;
    this.baysSeen = -1;
    this.dropEveryBay();
    this.carsAt.clear();
    this.bodies.clear();
    this.lorriesAt.clear();
    this.waiting.clear();
    this.carBySlot.length = 0;
    this.stays.clear();
    this.buildingsSeen = -1;
    // As a new map's: the counters from nought
    this.called = 0;
    this.parkedIn = 0;
    this.leftBy = 0;
  }

// src/sim/agents/lotTraffic.ts:220-252 slice of src/sim/agents/lotTraffic.ts#LotTraffic.step (shared): the feature's statements; "…" marks code left out
  step(w: SimWorld): void {
    if (!w.ambient.enabled) return;
    this.ambient = w.ambient;
    w.ambient.sceneryWatch = this.sceneryMoved;
  // … 28 lines
  }

// src/sim/agents/lotTraffic.ts:268-307 slice of src/sim/agents/lotTraffic.ts#LotTraffic.depart (shared): the feature's statements; "…" marks code left out
  private depart(w: SimWorld, target: number): void {
    // … 8 lines
    KINDS.forEach((kind, k) => { weights[k] = flows(kind, hour).out; });
    // … 1 lines
    for (let k = 0; k < short && this.waiting.total() + this.scenery.total() > 0; k++) {
      // … 22 lines
      const body = w.ambient.release(w, w.ambient.sceneryCarAt(bay!.key)!);
      this.track(body.id, bay!.bay, 'parked', body, true);
      // … 1 lines
      w.ambient.addParked(body);
    }
  }

// src/sim/agents/lotTraffic.ts:326-367 slice of src/sim/agents/lotTraffic.ts#LotTraffic.readBays (shared): the feature's statements; "…" marks code left out
  private readBays(w: SimWorld): void {
    // … 11 lines
    for (const source of new Set(sources)) {
      for (const bay of this.bySource.get(source) ?? []) {
        const key = spotKey(bay.x, bay.y);
        gone.add(key);
        this.dropBay(key);
      }
    // … 3 lines
    }
  // … 19 lines
  }

// src/sim/agents/lotTraffic.ts:370-387 slice of src/sim/agents/lotTraffic.ts#LotTraffic.fileBay (shared): the feature's statements; "…" marks code left out
  private fileBay(w: SimWorld, bay: Bay): void {
    const key = spotKey(bay.x, bay.y);
    let e = this.bays.get(key);
    if (e) {
      this.leaveBuilding(e);
      e.bay = bay;
    } else {
      e = { bay, key, slot: this.free.push(-1), kind: GONE };
      this.scenery.push(-1);
      this.bySlot[e.slot] = e;
      this.bays.set(key, e);
    }
    e.kind = kindClass(w, bay.building);
    // … 2 lines
    list.add(e);
    this.refreshBay(e);
  }

// src/sim/agents/lotTraffic.ts:440-447 slice of src/sim/agents/lotTraffic.ts#LotTraffic.followBuildings (shared): the feature's statements; "…" marks code left out
  private followBuildings(w: SimWorld): void {
    // … 2 lines
    const changed = buildingsChangedSince(diary, this.buildingsSeen);
    // … 1 lines
    if (changed === null) { this.rankEvery(w); return; }
    for (const id of changed) for (const e of this.baysOf.get(id) ?? []) this.rankBay(w, e);
  }

// src/sim/agents/lotTraffic.ts:468-473 slice of src/sim/agents/lotTraffic.ts#LotTraffic.refreshBay (shared): the feature's statements; "…" marks code left out
  private refreshBay(e: GatedBay): void {
    const standing = this.ambient?.sceneryCarAt(e.key) !== undefined;
    const free = !standing && !this.carsAt.has(e.key) && !this.bodies.has(e.key) && !this.lorriesAt.has(e.key);
    this.free.set(e.slot, free ? e.kind : -1);
    this.scenery.set(e.slot, standing ? e.kind : -1);
  }

// src/sim/agents/lotTraffic.ts:476-484 slice of src/sim/agents/lotTraffic.ts#LotTraffic.placeBody (shared): the feature's statements; "…" marks code left out
  private placeBody(car: LotCar): void {
    // … 1 lines
    const key = spotKey(f.x, f.y);
    if (key === car.bodyAt) return;
    // … 1 lines
    car.bodyAt = key;
    this.bodies.set(key, (this.bodies.get(key) ?? 0) + 1);
    this.refresh(key);
  }

// src/sim/agents/lotTraffic.ts:499-513 slice of src/sim/agents/lotTraffic.ts#LotTraffic.holdLorries (shared): the feature's statements; "…" marks code left out
  private holdLorries(hold: boolean): void {
    if (hold) {
      for (const p of this.lorries.places()) {
        const key = spotKey(p.x, p.y);
        if (this.lorriesAt.has(key)) continue;
        this.lorriesAt.add(key);
        this.refresh(key);
      }
    // … 1 lines
    }
  // … 4 lines
  }

// src/sim/agents/lotTraffic.ts:516-563 slice of src/sim/agents/lotTraffic.ts#LotTraffic.call (shared): the feature's statements; "…" marks code left out
  private call(w: SimWorld): boolean {
    // … 4 lines
    KINDS.forEach((kind, k) => { weights[k] = flows(kind, hour).in; });
  // … 41 lines
  }

// src/sim/agents/lotTraffic.ts:567-585 slice of src/sim/agents/lotTraffic.ts#LotTraffic.coming (shared): the feature's statements; "…" marks code left out
  private coming(w: SimWorld, car: LotCar): void {
    // … 14 lines
    w.ambient.addParked(body);
  // … 2 lines
  }

// src/sim/agents/lotTraffic.ts:672-675 slice of src/sim/agents/lotTraffic.ts#LotTraffic.drop (shared): the feature's statements; "…" marks code left out
  private drop(w: SimWorld, car: LotCar): void {
    // … 1 lines
    if (car.body) w.ambient.removeParked(car.body);
  }

// src/sim/agents/lotTraffic.ts:678-700 slice of src/sim/agents/lotTraffic.ts#LotTraffic.mayGo (shared): the feature's statements; "…" marks code left out
  private mayGo(w: SimWorld, car: LotCar, body: Vehicle): boolean {
    // … 16 lines
    const people = w.pedEngine.bridge;
    for (const d of discs) if (people.anyoneWithin(w, d.x, d.y, r, null)) { clear = false; break; }
    if (clear || car.waited >= ASK_WAY) { car.reserved = true; return true; }
  // … 2 lines
  }
