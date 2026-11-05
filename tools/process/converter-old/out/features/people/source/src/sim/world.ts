// The code of src/sim/world.ts that is the feature's (the rest of the file is shared).

// src/sim/world.ts:61-65 slice of src/sim/world.ts#NO_PEDESTRIANS (shared): the feature's statements; "…" marks code left out
const NO_PEDESTRIANS: PedestrianEngine = {
  kind: 'people',
  beginTick() {}, dispatch() {}, step() {}, rebind() {}, publish() {}, audit() {}, reset() {},
  bridge: { hailable: () => null, board: () => null, alight: () => false, anyoneWithin: () => false },
};

// src/sim/world.ts:157-157 slice of src/sim/world.ts#SimWorld.sidewalks (shared): the feature's statements; "…" marks code left out
  readonly sidewalks = new SidewalkGraph();

// src/sim/world.ts:159-159 slice of src/sim/world.ts#SimWorld.signalPosts (shared): the feature's statements; "…" marks code left out
  readonly signalPosts = new SignalPostIndex();

// src/sim/world.ts:182-182 slice of src/sim/world.ts#SimWorld.pedEngine (shared): the feature's statements; "…" marks code left out
  pedEngine: PedestrianEngine = NO_PEDESTRIANS;

// src/sim/world.ts:204-204 slice of src/sim/world.ts#SimWorld.crossingStates (shared): the feature's statements; "…" marks code left out
  readonly crossingStates: CrossingStates = new Map();

// src/sim/world.ts:206-206 slice of src/sim/world.ts#SimWorld.pedViews (shared): the feature's statements; "…" marks code left out
  readonly pedViews: PedView[] = [];

// src/sim/world.ts:208-208 slice of src/sim/world.ts#SimWorld.pedViewById (shared): the feature's statements; "…" marks code left out
  readonly pedViewById = new Map<number, PedView>();

// src/sim/world.ts:210-210 slice of src/sim/world.ts#SimWorld.crossingSpans (shared): the feature's statements; "…" marks code left out
  readonly crossingSpans = new CrossingSpans();

// src/sim/world.ts:231-231 field SimWorld.nextPersonId: the feature's own
  nextPersonId = 1;

// src/sim/world.ts:252-252 field SimWorld.pedestrianCount: the feature's own
  pedestrianCount: number | null = null;

// src/sim/world.ts:267-267 slice of src/sim/world.ts#SimWorld.ambient (shared): the feature's statements; "…" marks code left out
  readonly ambient = new AmbientWorld();

// src/sim/world.ts:304-310 slice of src/sim/world.ts#SimWorld.usePedestrianEngine (shared): the feature's statements; "…" marks code left out
  usePedestrianEngine(engine: PedestrianEngine): void {
    // … 3 lines
    this.pedEngine = engine;
    engine.rebind(this);
  }

// src/sim/world.ts:313-357 slice of src/sim/world.ts#SimWorld.reset (shared): the feature's statements; "…" marks code left out
  reset(): void {
    // … 12 lines
    this.sidewalks.clear();
    // … 1 lines
    this.signalPosts.reset();
    // … 26 lines
    this.pedEngine.reset(this);
  // … 1 lines
  }

// src/sim/world.ts:461-475 method SimWorld.someVehicleNear: the feature's own
  someVehicleNear(x: number, y: number, reach: number, test: (v: Vehicle) => boolean): boolean {
    const lanes = this.nearLanes;
    lanesNearInto(this, x, y, reach + BODY_OFF_LANE, lanes);
    try {
      for (const lane of lanes) {
        for (const id of this.rt(lane.id).order) {
          const v = this.vehicles.get(id);
          if (v && test(v)) return true;
        }
      }
      return false;
    } finally {
      lanes.length = 0;
    }
  }

// src/sim/world.ts:595-599 method SimWorld.signalAtCrossing: the feature's own
  signalAtCrossing(x: CrossingId): void {
    const node = crossingNode(x);
    const c = this.asleep.get(node);
    if (c && c.plan.stages.some((stage) => stage.pedWalk.includes(x))) this.wakeSignal(node);
  }

// src/sim/world.ts:644-672 slice of src/sim/world.ts#SimWorld.prepareVehicleTopology (shared): the feature's statements; "…" marks code left out
  *prepareVehicleTopology(measurer?: PairMeasurer): Generator<void | typeof WAITING, void> {
    // … 25 lines
    yield* this.walkways.updateSteps(this.net, measurer);
  // … 1 lines
  }

// src/sim/world.ts:683-683 slice of src/sim/world.ts#SimWorld.walkways (shared): the feature's statements; "…" marks code left out
  private readonly walkways = new Walkways();

// src/sim/world.ts:685-687 slice of src/sim/world.ts#SimWorld.walkwaysNow (shared): the feature's statements; "…" marks code left out
  walkwaysNow(): WalkGraph {
    return this.walkways.update(this.net);
  }

// src/sim/world.ts:947-961 slice of src/sim/world.ts#SimWorld.rebuildWalkTopology (shared): the feature's statements; "…" marks code left out
  rebuildWalkTopology(): void {
    // … 9 lines
    this.sidewalks.update(this.doc, this.net, this.graph, changes);
    this.crossingSpans.update(this, nodes);
  // … 2 lines
  }

// src/sim/world.ts:964-973 slice of src/sim/world.ts#SimWorld.crossingsAt (shared): the feature's statements; "…" marks code left out
  crossingsAt(node: NodeId): CrossingId[] {
    // … 3 lines
    return n.incident
      .slice()
      .sort((a, b) => a - b)
      .map((seg) => makeCrossingId(node, seg))
      .filter((id) => this.sidewalks.crossings.has(id));
  }

// src/sim/world.ts:983-1054 slice of src/sim/world.ts#SimWorld.makeSignalDeps (shared): the feature's statements; "…" marks code left out
  private makeSignalDeps(): { readonly deps: WorldSignalDeps; readonly fresh: () => void } {
    // … 40 lines
    const deps: WorldSignalDeps = {
      // … 9 lines
      pedestriansCrossing: (_node, crossings) =>
        crossings.some((x) => (this.crossingStates.get(x)?.occupants.length ?? 0) > 0),
      // … 2 lines
      pedestrianWait: (_node, crossings) => {
        // … 1 lines
        for (const x of crossings) longest = Math.max(longest, this.crossingStates.get(x)?.longestWait ?? 0);
        return longest;
      },
      pedestrianDemandOn: (_node, crossings) => crossings.some((x) => this.crossingStates.get(x)?.demand ?? false),
    // … 8 lines
    };
  // … 1 lines
  }

// src/sim/world.ts:1090-1101 slice of src/sim/world.ts#SimWorld.junctionQuiet (shared): the feature's statements; "…" marks code left out
  private junctionQuiet(c: SignalController, reserved: () => ReadonlyMap<NodeId, ReadonlySet<number>>): boolean {
    // … 3 lines
    for (const stage of c.plan.stages) {
      for (const x of stage.pedWalk) {
        // … 1 lines
        if (at && (at.occupants.length > 0 || at.demand || at.waitingFrom > 0 || at.waitingTo > 0)) return false;
      }
    }
  // … 1 lines
  }

// src/sim/world.ts:1311-1311 function crossingNode: the feature's own
const crossingNode = (id: CrossingId): NodeId => Number(id.slice(0, id.indexOf(':'))) as NodeId;
