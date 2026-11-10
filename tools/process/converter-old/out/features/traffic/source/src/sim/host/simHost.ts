// The code of src/sim/host/simHost.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simHost.ts:41-41 slice of src/sim/host/simHost.ts#SimHost.buildings (shared): the feature's statements; "…" marks code left out
  private readonly buildings = new TrafficBuildingTable(this.doc.changes);

// src/sim/host/simHost.ts:44-44 slice of src/sim/host/simHost.ts#SimHost.meso (shared): the feature's statements; "…" marks code left out
  private readonly meso = new Meso();

// src/sim/host/simHost.ts:56-56 slice of src/sim/host/simHost.ts#SimHost.governor (shared): the feature's statements; "…" marks code left out
  private readonly governor = new PopulationGovernor(1);

// src/sim/host/simHost.ts:67-67 slice of src/sim/host/simHost.ts#SimHost.odometer (shared): the feature's statements; "…" marks code left out
  private readonly odometer = new WheelOdometer();

// src/sim/host/simHost.ts:87-111 slice of src/sim/host/simHost.ts#SimHost.handle (shared): the feature's statements; "…" marks code left out
  async handle(message: ToSim): Promise<void> {
    switch (message.type) {
      // … 5 lines
      case 'resync': takeTrafficMap(this.doc, this.buildings, message.map); return;
      // … 2 lines
      case 'delta':
        // … 1 lines
        this.buildings.apply(trafficChangesOf(message.buildings));
        return;
      // … 1 lines
      case 'window':
        if (message.whole) this.meso.requestWhole();
        this.meso.want(message.x, message.y, message.r);
        return;
    // … 4 lines
    }
  }

// src/sim/host/simHost.ts:113-144 slice of src/sim/host/simHost.ts#SimHost.init (shared): the feature's statements; "…" marks code left out
  private async init(map: TrafficMap, seed: number, zones: ReadonlyMap<string, Float64Array | null>, data: SharedArrayBuffer, control: SharedArrayBuffer,
    prefix: string | null, ambient: 'view' | 'edges', bake: NetworkBake | null): Promise<void> {
    // … 2 lines
    takeTrafficMap(this.doc, this.buildings, map);
    // … 3 lines
    const sim = new SimWorld(this.doc, this.net, seed, this.buildings);
    // … 1 lines
    sim.useMicroWindow();
    // … 2 lines
    useBayWorkers(sim, this.bayPool);
    sim.auditEnabled = true;
    sim.auditLevel = 'cheap';
    sim.auditEvery = 60;
    sim.conflicts.seed(zones);
    // … 1 lines
    if (prefix) sim.conflicts.lookup = (keys) => readDerivedSome<Float64Array | null>(prefix, keys);
    // … 1 lines
    sim.usePedestrianEngine((await import('../agents/walk')).createAgentWalkEngine());
    sim.followCity = true;
    sim.ambient.enabled = true;
    sim.ambient.source = ambient;
    // … 1 lines
    sim.meso = this.meso;
    this.sim = sim;
  // … 3 lines
  }

// src/sim/host/simHost.ts:147-158 slice of src/sim/host/simHost.ts#SimHost.load (shared): the feature's statements; "…" marks code left out
  private load(map: TrafficMap, bake: NetworkBake | null): void {
    takeTrafficMap(this.doc, this.buildings, map);
  // … 9 lines
  }

// src/sim/host/simHost.ts:161-199 slice of src/sim/host/simHost.ts#SimHost.frame (shared): the feature's statements; "…" marks code left out
  private frame(wall: number, hold: boolean, controls: SimControls): void {
    // … 11 lines
    this.governor.setMax(controls.maxShare);
    if (controls.startShare !== undefined) this.governor.start(controls.startShare);
    // … 13 lines
    if (!held) {
      // … 1 lines
      laneGrid(sim);
      // … 3 lines
      if (sim.topologyToken() === topology) sim.populationShare = this.governor.feed(sim.clock.lastSteps, sim.clock.lastStepMs, performance.now() / 1000, sim.vehicles.size, sim.pedViews.length);
    }
  // … 3 lines
  }

// src/sim/host/simHost.ts:202-281 slice of src/sim/host/simHost.ts#SimHost.publish (shared): the feature's statements; "…" marks code left out
  private publish(alpha: number): void {
    // … 43 lines
    for (const v of sim.ambient.movers) {
      const f = v.free;
      if (f && (f.px !== f.x || f.py !== f.y || f.pangle !== f.angle)) writeVehicle(v, true);
    }
    // … 3 lines
    for (const p of sim.pedViews) {
      if (m >= MAX_PEDS) break;
    // … 11 lines
    }
    // … 6 lines
    const meso = this.meso.takeDelta();
    // … 1 lines
    if (meso) transfer.push(...deltaTransfer(meso));
    this.post({
      // … 1 lines
      parked, meso, stats: this.stats(), wounds: takeWounds(sim),
    // … 1 lines
    }, transfer.length ? transfer : undefined);
  }

// src/sim/host/simHost.ts:212-243 slice of src/sim/host/simHost.ts#SimHost.publish.writeVehicle (shared): the feature's statements; "…" marks code left out
    const writeVehicle = (v: Vehicle, parked: boolean): void => {
      if (n >= MAX_VEHICLES || !vehicleFramesInto(sim, v, this.frames)) return;
      const f = this.frames, at = vb + n * VEHICLE_STRIDE;
      const lane = v.free ? undefined : sim.lanelet(v.lanelet);
      data[at + V.id] = v.id;
      data[at + V.free] = parked ? 2 : v.free ? 1 : 0;
      data[at + V.bx] = f.bx; data[at + V.by] = f.by; data[at + V.bangle] = f.bangle;
      data[at + V.x] = f.x; data[at + V.y] = f.y; data[at + V.angle] = f.angle;
      data[at + V.yawBefore] = f.yawBefore; data[at + V.yawNow] = f.yawNow; data[at + V.jump] = f.jump ? 1 : 0;
      data[at + V.v] = v.v; data[at + V.prevV] = v.prev.v; data[at + V.age] = v.age;
      data[at + V.segment] = lane?.segment ?? -1;
      data[at + V.inSegment] = lane?.segment ?? (lane ? sim.connector(lane.id)?.inSegment ?? -1 : -1);
      data[at + V.indicator] = v.free ? 0 : indicatorSide(sim, v);
      data[at + V.curvature] = v.free ? 0 : pathCurvature(sim, v);
      data[at + V.odometer] = this.odometer.advance(v);
      data[at + V.seats] = v.seats;
      data[at + V.slain] = v.slain ? 1 : 0;
      data[at + V.lot] = v.free?.lot ?? -1;
      n++;
      // Doors and a kerb stop move every tick while they last: told each tick then. DO NOT make a key a vehicle a frame to compare.
      const busy = v.doors.length > 0 || v.kerbStop !== null;
      const was = this.vehicleSent.get(v.id);
      if (was && !busy && !was.busy && was.color === v.color && was.archetype === v.archetype.id && sameList(was.people, v.people) && sameList(was.peopleAge, v.peopleAge)) {
        was.seen = frame;
        return;
      }
      // The kerb stop as it is: posted below in this same call, the message copies it (DO NOT copy it a second time, a vehicle a frame).
      const rich: VehicleRich = { color: v.color, archetype: v.archetype.id, doors: v.doors.slice(), kerbStop: v.kerbStop,
        people: v.people.slice(), peopleAge: v.peopleAge.slice(), freeLot: v.free?.lot ?? null };
      this.vehicleSent.set(v.id, { color: rich.color, archetype: rich.archetype, busy, people: rich.people, peopleAge: rich.peopleAge, seen: frame });
      vehicles.push([v.id, rich]);
    };

// src/sim/host/simHost.ts:288-292 slice of src/sim/host/simHost.ts#SimHost.signals.layoutOf (shared): the feature's statements; "…" marks code left out
    const layoutOf = (node: NodeId): SignalLayout => {
      const junction = sim.graph.junctions.get(node);
      const live = junction?.signalised && sim.controller(node);
      return { node, groups: live ? junction.groups.map((g) => ({ id: g.id, segments: [...g.segments] })) : [] };
    };

// src/sim/host/simHost.ts:296-302 slice of src/sim/host/simHost.ts#SimHost.signals.tell (shared): the feature's statements; "…" marks code left out
    const tell = (node: NodeId, controller: SignalController): void => {
      const stage = `${this.planId(controller)}|${controller.stageIndex}|${controller.sub}`;
      if (this.lightsSent.get(node) === stage) return;
      this.lightsSent.set(node, stage);
      const junction = sim.graph.junctions.get(node);
      signals.push({ node, stage, states: (junction?.groups ?? []).map((g) => [g.id, signalStateFor(controller, g.id)] as const) });
    };

// src/sim/host/simHost.ts:321-323 slice of src/sim/host/simHost.ts#SimHost.tellEvery (shared): the feature's statements; "…" marks code left out
  private tellEvery(tell: (node: NodeId, controller: SignalController) => void): void {
    for (const [node, controller] of this.sim!.controllers) tell(node, controller);
  }

// src/sim/host/simHost.ts:325-329 slice of src/sim/host/simHost.ts#SimHost.planId (shared): the feature's statements; "…" marks code left out
  private planId(c: SignalController): number {
    let id = this.plans.get(c.plan);
    if (id === undefined) this.plans.set(c.plan, id = ++this.planSerial);
    return id;
  }

// src/sim/host/simHost.ts:332-352 slice of src/sim/host/simHost.ts#SimHost.parked (shared): the feature's statements; "…" marks code left out
  private parked(): ParkedChanges | null {
    // … 9 lines
    cars.forEach((car, k) => {
      const at = k * PARKED_STRIDE, f = car.free;
      out[at + PK.id] = car.id;
      out[at + PK.gone] = (whole || sim.ambient.isParked(car)) && f ? 0 : 1;
      out[at + PK.x] = f?.x ?? 0; out[at + PK.y] = f?.y ?? 0; out[at + PK.angle] = f?.angle ?? 0;
      out[at + PK.lot] = f?.lot ?? -1;
      out[at + PK.archetype] = this.archetypeIndex(car.archetype.id);
      out[at + PK.color] = parseInt(car.color.slice(1), 16);
    });
  // … 1 lines
  }

// src/sim/host/simHost.ts:354-367 slice of src/sim/host/simHost.ts#SimHost.stats (shared): the feature's statements; "…" marks code left out
  private stats(): SimStats {
    // … 5 lines
    if (++this.measuredDue % 300 === 0 && this.zonesPrefix) writeDerivedMany(this.zonesPrefix, sim.conflicts.takeMeasured());
    // … 1 lines
    return {
      // … 1 lines
      controllers: sim.controllers.size, lotsMoving: sim.city.lots.moving(), topology: sim.topologyToken(), issues,
      populationShare: this.governor.share, stepMs: this.governor.stepMs, measured,
    };
  }

// src/sim/host/simHost.ts:370-401 slice of src/sim/host/simHost.ts#SimHost.act (shared): the feature's statements; "…" marks code left out
  private act(act: SimAct): unknown {
    // … 2 lines
    switch (act.kind) {
      // … 7 lines
      case 'strand': {
        // … 1 lines
        if (v) strandVehicle(v);
      // … 1 lines
      }
      // … 1 lines
      case 'removeVehiclesNear': {
        // … 1 lines
        for (const v of [...sim.vehicles.values()]) {
          const f = v.free ?? null;
          // … 1 lines
          if (!f && !vehicleFramesInto(sim, v, frames)) continue;
          const x = f ? f.x : frames.x, y = f ? f.y : frames.y;
          if (Math.hypot(x - act.x, y - act.y) > act.r) continue;
        // … 2 lines
        }
      // … 1 lines
      }
    }
  }

// src/sim/host/simHost.ts:463-465 slice of src/sim/host/simHost.ts#archetypeMap (shared): the feature's statements; "…" marks code left out
function archetypeMap(): Map<string, number> {
  return new Map(ARCHETYPES.map((a, k) => [a.id, k]));
}
