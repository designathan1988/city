// The code of src/sim/host/simHost.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simHost.ts:39-39 slice of src/sim/host/simHost.ts#SimHost.net (shared): the feature's statements; "…" marks code left out
  private net = new Network(this.doc);

// src/sim/host/simHost.ts:113-144 slice of src/sim/host/simHost.ts#SimHost.init (shared): the feature's statements; "…" marks code left out
  private async init(map: TrafficMap, seed: number, zones: ReadonlyMap<string, Float64Array | null>, data: SharedArrayBuffer, control: SharedArrayBuffer,
    prefix: string | null, ambient: 'view' | 'edges', bake: NetworkBake | null): Promise<void> {
    // … 4 lines
    if (bake) this.net.offerBake(bake);
    this.net.rebuild({ full: true });
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
    // … 1 lines
    if (bake) this.net.offerBake(bake);
    this.net.rebuild({ full: true });
  // … 7 lines
  }

// src/sim/host/simHost.ts:161-199 slice of src/sim/host/simHost.ts#SimHost.frame (shared): the feature's statements; "…" marks code left out
  private frame(wall: number, hold: boolean, controls: SimControls): void {
    // … 13 lines
    if (!this.net.isBuilt()) this.net.rebuild();
    // … 8 lines
    const catching = this.topology.step(sim, this.net.trafficToken(), false);
    if (!catching) this.topology.stepWindow(sim, this.net.trafficToken());
    sim.topologyDeferred = catching;
  // … 12 lines
  }

// src/sim/host/simHost.ts:212-243 slice of src/sim/host/simHost.ts#SimHost.publish.writeVehicle (shared): the feature's statements; "…" marks code left out
    const writeVehicle = (v: Vehicle, parked: boolean): void => {
      // … 9 lines
      data[at + V.segment] = lane?.segment ?? -1;
      data[at + V.inSegment] = lane?.segment ?? (lane ? sim.connector(lane.id)?.inSegment ?? -1 : -1);
    // … 19 lines
    };

// src/sim/host/simHost.ts:288-292 slice of src/sim/host/simHost.ts#SimHost.signals.layoutOf (shared): the feature's statements; "…" marks code left out
    const layoutOf = (node: NodeId): SignalLayout => {
      const junction = sim.graph.junctions.get(node);
      const live = junction?.signalised && sim.controller(node);
      return { node, groups: live ? junction.groups.map((g) => ({ id: g.id, segments: [...g.segments] })) : [] };
    };

// src/sim/host/simHost.ts:296-302 slice of src/sim/host/simHost.ts#SimHost.signals.tell (shared): the feature's statements; "…" marks code left out
    const tell = (node: NodeId, controller: SignalController): void => {
      // … 3 lines
      const junction = sim.graph.junctions.get(node);
      signals.push({ node, stage, states: (junction?.groups ?? []).map((g) => [g.id, signalStateFor(controller, g.id)] as const) });
    };

// src/sim/host/simHost.ts:404-409 slice of src/sim/host/simHost.ts#SimHost.lotPlan (shared): the feature's statements; "…" marks code left out
  private lotPlan(): LotPlan | null {
    // … 2 lines
    this.net.rebuild();
    return planLots(this.doc, this.net);
  }

// src/sim/host/simHost.ts:412-416 slice of src/sim/host/simHost.ts#SimHost.ask (shared): the feature's statements; "…" marks code left out
  private ask(ask: SimAsk): unknown {
    // … 2 lines
    return answer(sim, this.doc, this.net, ask);
  }
