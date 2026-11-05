// The code of src/sim/world.ts that is the feature's (the rest of the file is shared).

// src/sim/world.ts:46-46 slice of src/sim/world.ts#WorldSignalDeps (shared): the feature's statements; "…" marks code left out
export type WorldSignalDeps = SignalDeps & { readonly reserved: () => ReadonlyMap<NodeId, ReadonlySet<number>> };

// src/sim/world.ts:58-58 slice of src/sim/world.ts#CAR_CLASS (shared): the feature's statements; "…" marks code left out
/** The body class a signal plan is protected for: an ordinary car. */const CAR_CLASS: BodyClass = 1;

// src/sim/world.ts:78-88 slice of src/sim/world.ts#LaneletRuntime (shared): the feature's statements; "…" marks code left out
export interface LaneletRuntime {
  readonly id: LaneletId;
  // … 1 lines
  order: VehicleId[];
  // … 1 lines
  shadows: VehicleId[];
// … 4 lines
}

// src/sim/world.ts:134-140 slice of src/sim/world.ts#NO_RUNTIME (shared): the feature's statements; "…" marks code left out
const NO_RUNTIME: LaneletRuntime = Object.freeze({
  id: '' as LaneletId,
  order: Object.freeze([]) as unknown as VehicleId[],
  shadows: Object.freeze([]) as unknown as VehicleId[],
// … 2 lines
});

// src/sim/world.ts:155-155 slice of src/sim/world.ts#SimWorld.graph (shared): the feature's statements; "…" marks code left out
  readonly graph = new LaneletGraph();

// src/sim/world.ts:156-156 slice of src/sim/world.ts#SimWorld.conflicts (shared): the feature's statements; "…" marks code left out
  readonly conflicts = new ConflictIndex();

// src/sim/world.ts:160-160 slice of src/sim/world.ts#SimWorld.claims (shared): the feature's statements; "…" marks code left out
  readonly claims = new ClaimTable();

// src/sim/world.ts:163-167 slice of src/sim/world.ts#SimWorld.vehicles (shared): the feature's statements; "…" marks code left out
  readonly vehicles = new VersionedMap<VehicleId, Vehicle>({
    set: (v, was) => { if (was && was !== v) this.routes.unfile(was); this.routes.file(v); },
    delete: (v) => this.routes.unfile(v),
    clear: () => this.routes.clear(),
  });

// src/sim/world.ts:169-169 slice of src/sim/world.ts#SimWorld.routes (shared): the feature's statements; "…" marks code left out
  readonly routes = new RouteIndex<Vehicle>();

// src/sim/world.ts:171-171 slice of src/sim/world.ts#SimWorld.rebindLanes (shared): the feature's statements; "…" marks code left out
  private readonly rebindLanes = new Set<LaneletId>();

// src/sim/world.ts:186-186 slice of src/sim/world.ts#SimWorld.runtime (shared): the feature's statements; "…" marks code left out
  readonly runtime = new Map<LaneletId, LaneletRuntime>();

// src/sim/world.ts:187-187 slice of src/sim/world.ts#SimWorld.controllers (shared): the feature's statements; "…" marks code left out
  readonly controllers = new Map<NodeId, SignalController>();

// src/sim/world.ts:191-191 slice of src/sim/world.ts#SimWorld.occupied (shared): the feature's statements; "…" marks code left out
  readonly occupied = new Set<LaneletId>();

// src/sim/world.ts:193-193 field SimWorld.clearancesAtStart: the feature's own
  readonly clearancesAtStart = new Set<Vehicle['clearingConnectors'][number]>();

// src/sim/world.ts:195-195 slice of src/sim/world.ts#SimWorld.maybeHolders (shared): the feature's statements; "…" marks code left out
  private readonly maybeHolders = new Set<Vehicle>();

// src/sim/world.ts:197-197 slice of src/sim/world.ts#SimWorld.ghostCandidates (shared): the feature's statements; "…" marks code left out
  readonly ghostCandidates = new Set<LaneletId>();

// src/sim/world.ts:199-199 slice of src/sim/world.ts#SimWorld.asleep (shared): the feature's statements; "…" marks code left out
  private readonly asleep = new Map<NodeId, SignalController>();

// src/sim/world.ts:201-201 slice of src/sim/world.ts#SimWorld.signalClock (shared): the feature's statements; "…" marks code left out
  readonly signalClock: SignalClock = { passes: 0 };

// src/sim/world.ts:210-210 slice of src/sim/world.ts#SimWorld.crossingSpans (shared): the feature's statements; "…" marks code left out
  readonly crossingSpans = new CrossingSpans();

// src/sim/world.ts:215-215 slice of src/sim/world.ts#SimWorld.mergeTurn (shared): the feature's statements; "…" marks code left out
  readonly mergeTurn = new Map<LaneletId, LaneletId>();

// src/sim/world.ts:219-219 slice of src/sim/world.ts#SimWorld.flow (shared): the feature's statements; "…" marks code left out
  get flow(): FlowStats { return this.flowStats; }

// src/sim/world.ts:220-220 slice of src/sim/world.ts#SimWorld.flowStats (shared): the feature's statements; "…" marks code left out
  private flowStats = new FlowStats();

// src/sim/world.ts:222-222 slice of src/sim/world.ts#SimWorld.advisor (shared): the feature's statements; "…" marks code left out
  get advisor(): ControlAdvisor { return this.controlAdvisor; }

// src/sim/world.ts:223-223 slice of src/sim/world.ts#SimWorld.controlAdvisor (shared): the feature's statements; "…" marks code left out
  private controlAdvisor = new ControlAdvisor();

// src/sim/world.ts:231-231 field SimWorld.nextPersonId: the feature's own
  nextPersonId = 1;

// src/sim/world.ts:233-233 field SimWorld.roadLength: the feature's own
  readonly roadLength = new NetworkLength();

// src/sim/world.ts:237-237 slice of src/sim/world.ts#SimWorld.laneReaches (shared): the feature's statements; "…" marks code left out
  readonly laneReaches = new LaneReaches();

// src/sim/world.ts:242-242 slice of src/sim/world.ts#SimWorld.entryDemand (shared): the feature's statements; "…" marks code left out
  readonly entryDemand = new Map<LaneletId, { next: number; rate: number; waiting: number[] }>();

// src/sim/world.ts:262-265 slice of src/sim/world.ts#SimWorld.reservationsChecked (shared): the feature's statements; "…" marks code left out
  readonly reservationsChecked = new WeakMap<Vehicle, {
    readonly lanelet: LaneletId; readonly admitted: LaneletId | null; readonly reserved: readonly LaneletId[]; readonly reservedLength: number;
    readonly route: readonly LaneletId[]; readonly routeLength: number; readonly topology: object;
  }>();

// src/sim/world.ts:293-301 slice of src/sim/world.ts#SimWorld.constructor (shared): the feature's statements; "…" marks code left out
  constructor(
    // … 4 lines
    readonly buildings: TrafficBuildings = docBuildings(doc),
  // … 2 lines
  }

// src/sim/world.ts:313-357 slice of src/sim/world.ts#SimWorld.reset (shared): the feature's statements; "…" marks code left out
  reset(): void {
    // … 5 lines
    this.claims.clear();
    // … 3 lines
    this.graph.forget();
    this.prepGraph?.forget();
    // … 2 lines
    this.walkSeen = null;
    // … 7 lines
    this.signalClock.passes = 0;
    // … 6 lines
    this.entryDemand.clear();
    // … 7 lines
    this.flowStats = new FlowStats();
    this.controlAdvisor = new ControlAdvisor();
    // … 4 lines
    this.meso?.reset();
  }

// src/sim/world.ts:361-363 slice of src/sim/world.ts#SimWorld.lanelet (shared): the feature's statements; "…" marks code left out
  lanelet(id: LaneletId): Lanelet | undefined {
    return this.graph.lanelets.get(id);
  }

// src/sim/world.ts:371-373 slice of src/sim/world.ts#SimWorld.connector (shared): the feature's statements; "…" marks code left out
  connector(id: string): Connector | undefined {
    return this.graph.connectors.get(id);
  }

// src/sim/world.ts:376-378 slice of src/sim/world.ts#SimWorld.rt (shared): the feature's statements; "…" marks code left out
  rt(id: LaneletId): LaneletRuntime {
    return this.runtime.get(id) ?? NO_RUNTIME;
  }

// src/sim/world.ts:381-389 slice of src/sim/world.ts#SimWorld.runtimeFor (shared): the feature's statements; "…" marks code left out
  private runtimeFor(id: LaneletId): LaneletRuntime {
    let r = this.runtime.get(id);
    if (!r) {
      r = { id, order: [], shadows: [], ghost: false, ghostSince: 0 };
      this.runtime.set(id, r);
      if (!this.graph.lanelets.has(id)) this.ghostCandidates.add(id);
    }
    return r;
  }

// src/sim/world.ts:391-393 slice of src/sim/world.ts#SimWorld.veh (shared): the feature's statements; "…" marks code left out
  veh(id: VehicleId): Vehicle | undefined {
    return this.vehicles.get(id);
  }

// src/sim/world.ts:395-397 slice of src/sim/world.ts#SimWorld.controller (shared): the feature's statements; "…" marks code left out
  controller(node: NodeId): SignalController | undefined {
  // … 1 lines
  }

// src/sim/world.ts:400-402 slice of src/sim/world.ts#SimWorld.noteRoute (shared): the feature's statements; "…" marks code left out
  noteRoute(v: Vehicle): void {
    if (this.vehicles.get(v.id) === v) this.routes.file(v);
  }

// src/sim/world.ts:405-426 slice of src/sim/world.ts#SimWorld.vehiclesReached (shared): the feature's statements; "…" marks code left out
  vehiclesReached(): readonly Vehicle[] | null {
    // … 3 lines
    const out = new Set<Vehicle>();
    // … 1 lines
    for (const id of lanes) {
      // … 2 lines
      for (const v of this.routes.routing(id) ?? []) out.add(v);
    }
    // … 1 lines
    if (lanes.size) {
      for (const v of this.junctionHolders()) {
        if ((v.admittedConnector !== null && lanes.has(v.admittedConnector)) ||
            v.reservedConnectors.some((id) => lanes.has(id)) ||
            v.clearingConnectors.some((token) => lanes.has(token.connector))) out.add(v);
      }
    }
    // … 1 lines
    return [...out].sort((a, b) => a.id - b.id);
  }

// src/sim/world.ts:410-410 slice of src/sim/world.ts#SimWorld.vehiclesReached.take (shared): the feature's statements; "…" marks code left out
    const take = (id: VehicleId): void => { const v = this.vehicles.get(id); if (v) out.add(v); };

// src/sim/world.ts:429-431 slice of src/sim/world.ts#SimWorld.noteHolder (shared): the feature's statements; "…" marks code left out
  noteHolder(v: Vehicle): void {
    this.maybeHolders.add(v);
  }

// src/sim/world.ts:434-442 slice of src/sim/world.ts#SimWorld.junctionHolders (shared): the feature's statements; "…" marks code left out
  junctionHolders(): Vehicle[] {
    const out: Vehicle[] = [];
    for (const v of this.maybeHolders) {
      if (this.vehicles.get(v.id) !== v) { this.maybeHolders.delete(v); continue; }
      if (v.admittedConnector !== null || v.clearingConnectors.length > 0 || this.lanelet(v.lanelet)?.kind === 'connector') out.push(v);
      else this.maybeHolders.delete(v);
    }
    return out;
  }

// src/sim/world.ts:445-458 method SimWorld.vehiclesNear: the feature's own
  vehiclesNear(x: number, y: number, reach: number): Vehicle[] {
    const out: Vehicle[] = [];
    // The lanes in no order: the vehicles are sorted by id after (SIM-28).
    const lanes = this.nearLanes;
    lanesNearInto(this, x, y, reach + BODY_OFF_LANE, lanes);
    for (const lane of lanes) {
      for (const id of this.rt(lane.id).order) {
        const v = this.vehicles.get(id);
        if (v) out.push(v);
      }
    }
    lanes.length = 0;
    return out.sort((a, b) => a.id - b.id);
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

// src/sim/world.ts:478-478 field SimWorld.nearLanes: the feature's own
  private readonly nearLanes: Lanelet[] = [];

// src/sim/world.ts:481-483 slice of src/sim/world.ts#SimWorld.vehiclesInIdOrder (shared): the feature's statements; "…" marks code left out
  vehiclesInIdOrder(): readonly Vehicle[] {
  // … 1 lines
  }

// src/sim/world.ts:491-493 slice of src/sim/world.ts#SimWorld.signalisedControllers (shared): the feature's statements; "…" marks code left out
  signalisedControllers(): readonly SignalController[] {
  // … 1 lines
  }

// src/sim/world.ts:496-496 slice of src/sim/world.ts#SimWorld.ordered (shared): the feature's statements; "…" marks code left out
  private ordered: { readonly nodes: readonly NodeId[]; readonly signalised: readonly SignalController[] } | null = null;

// src/sim/world.ts:498-507 slice of src/sim/world.ts#SimWorld.inNodeOrder (shared): the feature's statements; "…" marks code left out
  private inNodeOrder(): { readonly nodes: readonly NodeId[]; readonly signalised: readonly SignalController[] } {
    // … 1 lines
    const nodes = [...this.graph.junctions.keys()].sort((a, b) => a - b);
    const signalised: SignalController[] = [];
    for (const node of nodes) {
      const c = this.controllers.get(node);
      if (c && this.graph.junctions.get(node)!.signalised) signalised.push(c);
    }
    return this.ordered = { nodes, signalised };
  }

// src/sim/world.ts:519-523 slice of src/sim/world.ts#SimWorld.junctionOfLane (shared): the feature's statements; "…" marks code left out
  private junctionOfLane(id: LaneletId): NodeId | undefined {
    const lane = this.graph.lanelets.get(id);
    if (!lane || lane.kind !== 'link' || lane.to === undefined) return undefined;
    return this.graph.junctions.get(lane.to)?.inbound.includes(id) ? lane.to : undefined;
  }

// src/sim/world.ts:526-538 method SimWorld.junctionsApproached: the feature's own
  junctionsApproached(): readonly NodeId[] {
    const out = this.approached, seen = this.approachedSeen;
    seen.clear();
    let n = 0;
    for (const lane of this.occupied) {
      const node = this.junctionOfLane(lane);
      if (node === undefined || seen.has(node)) continue;
      seen.add(node);
      out[n++] = node;
    }
    out.length = n;
    return out.sort((a, b) => a - b);
  }

// src/sim/world.ts:539-539 field SimWorld.approached: the feature's own
  private readonly approached: NodeId[] = [];

// src/sim/world.ts:540-540 field SimWorld.approachedSeen: the feature's own
  private readonly approachedSeen = new Set<NodeId>();

// src/sim/world.ts:546-581 slice of src/sim/world.ts#SimWorld.signalsToStep (shared): the feature's statements; "…" marks code left out
  signalsToStep(deps: WorldSignalDeps): readonly SignalController[] {
    // … 8 lines
    if (this.wakeAll || entries === null || entries.some((e) => e.rects === null)) this.wakeEverySignal();
    else {
      for (const node of this.wake) {
        // … 3 lines
        if (c && this.graph.junctions.get(node)?.signalised) awake.set(node, c);
      }
    // … 3 lines
    }
    // … 13 lines
    return out.sort((a, b) => a.node - b.node);
  }

// src/sim/world.ts:582-582 slice of src/sim/world.ts#SimWorld.stepping (shared): the feature's statements; "…" marks code left out
  private readonly stepping: SignalController[] = [];

// src/sim/world.ts:585-585 slice of src/sim/world.ts#SimWorld.awake (shared): the feature's statements; "…" marks code left out
  private readonly awake = new Map<NodeId, SignalController>();

// src/sim/world.ts:588-592 slice of src/sim/world.ts#SimWorld.wakeEverySignal (shared): the feature's statements; "…" marks code left out
  private wakeEverySignal(): void {
    // … 2 lines
    for (const c of this.signalisedControllers()) this.awake.set(c.node, c);
  }

// src/sim/world.ts:595-599 method SimWorld.signalAtCrossing: the feature's own
  signalAtCrossing(x: CrossingId): void {
    const node = crossingNode(x);
    const c = this.asleep.get(node);
    if (c && c.plan.stages.some((stage) => stage.pedWalk.includes(x))) this.wakeSignal(node);
  }

// src/sim/world.ts:612-614 slice of src/sim/world.ts#SimWorld.noteSignalStepped (shared): the feature's statements; "…" marks code left out
  noteSignalStepped(c: SignalController): void {
    this.signalsStepped.add(c.node);
  }

// src/sim/world.ts:625-629 slice of src/sim/world.ts#SimWorld.restSignal (shared): the feature's statements; "…" marks code left out
  restSignal(c: SignalController): void {
    if (this.awake.get(c.node) !== c) return;
    this.awake.delete(c.node);
    this.asleep.set(c.node, c);
  }

// src/sim/world.ts:641-641 slice of src/sim/world.ts#SimWorld.prepGraph (shared): the feature's statements; "…" marks code left out
  private prepGraph: LaneletGraph | null = null;

// src/sim/world.ts:644-672 slice of src/sim/world.ts#SimWorld.prepareVehicleTopology (shared): the feature's statements; "…" marks code left out
  *prepareVehicleTopology(measurer?: PairMeasurer): Generator<void | typeof WAITING, void> {
    const graph = this.prepGraph ??= new LaneletGraph();
    // … 1 lines
    const left = graph.turnPlan;
    if (left) left.recording = measurer?.chooseTurns !== undefined;
    graph.turnPlan = measurer?.chooseTurns ? left ?? new TurnPlan() : left;
    // … 1 lines
    const changed = this.prepReach(yield* graph.updateSteps(this.doc, this.net, measurer ?? null));
    const plan = graph.turnPlan;
    if (plan && plan.asks.size > 0) {
      const keys = [...plan.asks.keys()];
      const answer: { turns?: readonly ChosenTurn[]; failed?: boolean } = {};
      measurer!.chooseTurns!(keys.map((k) => plan.asks.get(k)!), plan.surfaces).then(
        (turns) => { answer.turns = turns; },
        (error: unknown) => { console.warn('[traffic] the turn workers failed; the turns are chosen on the page', error); answer.failed = true; },
      );
      while (!answer.turns && !answer.failed) yield WAITING;
      keys.forEach((k, i) => { const turn = answer.turns?.[i]; if (turn) plan.chosen.set(k, turn); });
    }
    // … 1 lines
    graph.redoProvisional(this.doc, this.net);
    // … 2 lines
    yield* this.conflicts.prepare(graph, measurer, changed);
    // … 2 lines
    yield* this.walkways.updateSteps(this.net, measurer);
  // … 1 lines
  }

// src/sim/world.ts:676-680 slice of src/sim/world.ts#SimWorld.prepReach (shared): the feature's statements; "…" marks code left out
  private prepReach(changed: GraphChange | null): ReadonlySet<NodeId> | null {
    if (changed === null || this.prepReached === null) this.prepReached = null;
    else for (const node of changed.junctions) this.prepReached.add(node);
  // … 1 lines
  }

// src/sim/world.ts:700-700 slice of src/sim/world.ts#SimWorld.topologyLog (shared): the feature's statements; "…" marks code left out
  readonly topologyLog = new ChangeLog<GraphChange>();

// src/sim/world.ts:705-713 slice of src/sim/world.ts#SimWorld.fileJunctions (shared): the feature's statements; "…" marks code left out
  private fileJunctions(changed: GraphChange | null): void {
    const nodes = changed === null ? [...this.junctionFiling.ids(), ...this.graph.junctions.keys()] : changed.junctions;
    for (const node of nodes) {
      this.junctionFiling.remove(node);
      const at = this.graph.junctions.has(node) ? this.doc.node(node) : undefined;
      if (at) this.junctionFiling.file(node, JUNCTION_FILED, [at.x, at.y, at.x, at.y]);
    }
  // … 1 lines
  }

// src/sim/world.ts:716-720 slice of src/sim/world.ts#SimWorld.useMicroWindow (shared): the feature's statements; "…" marks code left out
  useMicroWindow(): void {
    // … 2 lines
    this.windowMoved(this.conflicts.setWindow(this.graph, new Set()));
  }

// src/sim/world.ts:723-731 slice of src/sim/world.ts#SimWorld.moveWindowSteps (shared): the feature's statements; "…" marks code left out
  *moveWindowSteps(measurer?: PairMeasurer): Generator<void | typeof WAITING, void> {
    // … 3 lines
    const entering = this.conflicts.entering(this.graph, next);
    // … 1 lines
    if (entering.size) yield* this.conflicts.prepare(this.graph, measurer, entering, next);
    this.windowMoved(this.conflicts.setWindow(this.graph, next));
  }

// src/sim/world.ts:734-739 slice of src/sim/world.ts#SimWorld.windowMoved (shared): the feature's statements; "…" marks code left out
  private windowMoved(changed: ReadonlySet<NodeId>): void {
    // … 1 lines
    this.claims.dropMissing(this.conflicts);
  // … 2 lines
  }

// src/sim/world.ts:755-778 slice of src/sim/world.ts#SimWorld.keepMicroWindow (shared): the feature's statements; "…" marks code left out
  keepMicroWindow(): void {
    // … 2 lines
    if (tick - this.windowTick < Math.round(WINDOW_EVERY / DT) && this.vehicles.size <= this.windowFleet) return;
    // … 1 lines
    this.windowFleet = this.vehicles.size;
    // … 16 lines
    this.windowWanted = this.conflicts.holdsWindow(next) ? null : next;
  }

// src/sim/world.ts:781-825 slice of src/sim/world.ts#SimWorld.rebuildVehicleTopology (shared): the feature's statements; "…" marks code left out
  rebuildVehicleTopology(): void {
    // … 1 lines
    const prep = this.prepGraph;
    const changed = prep?.current(this.net) ? this.graph.adopt(prep, this.doc, this.net) : this.graph.update(this.doc, this.net);
    this.topologyLog.push(changed);
    // … 1 lines
    this.laneReaches.follow(this);
    // … 1 lines
    if (this.vehicles.size === 0) { this.rebindLanes.clear(); this.rebindAll = false; }
    else if (changed === null) this.rebindAll = true;
    else for (const id of changed.lanelets) this.rebindLanes.add(id);
    this.fileJunctions(changed);
    if (changed === null) this.conflicts.build(this.graph); else this.conflicts.update(this.graph, changed.junctions);
    this.claims.dropMissing(this.conflicts);
    this.syncControllers(changed?.junctions ?? null);
    for (const segment of changed ? [...changed.segments] : [...this.segmentVolume.keys()]) {
      rebuildWork.lanes++;
      if (!this.doc.segment(segment as SegmentId)) this.segmentVolume.delete(segment);
    }
    // … 2 lines
    for (const id of changed ? changed.lanelets : this.runtime.keys()) {
      const rt = this.runtime.get(id);
      if (!rt) continue;
      rebuildWork.lanes++;
      if (!this.graph.lanelets.has(id)) this.ghostCandidates.add(id);
      else if (rt.ghost) { rt.ghost = false; rt.ghostSince = 0; }
    }
    this.junctionsChanged(changed?.junctions ?? null);
    // … 1 lines
    if (changed === null) {
      const nodes = new Set(this.graph.junctions.keys());
      rebuildWork.lanes += nodes.size;
      this.flow.keepOnly(nodes);
      this.advisor.forget(nodes);
      for (const node of nodes) this.advised.add(node);
    } else {
      const gone = new Set([...changed.junctions].filter((node) => !this.graph.junctions.has(node)));
      if (gone.size) { this.flow.drop(gone); this.advisor.drop(gone); }
      for (const node of changed.junctions) this.advised.add(node);
    }
    // … 2 lines
    if (this.applyAutoControl(changed?.junctions)) this.syncControllers(changed?.junctions ?? null);
  }

// src/sim/world.ts:841-852 slice of src/sim/world.ts#SimWorld.noteSignal (shared): the feature's statements; "…" marks code left out
  private noteSignal(node: NodeId): void {
    if (this.graph.junctions.get(node)?.signalised) this.signalised.add(node); else this.signalised.delete(node);
    // … 3 lines
    const most = 2 * Math.max(64, this.graph.junctions.size);
    if (this.signalLog.length > most) {
      const drop = this.signalLog.length - most / 2;
      this.signalLog.splice(0, drop);
      this.signalLogBase += drop;
    }
  }

// src/sim/world.ts:859-878 slice of src/sim/world.ts#SimWorld.noteJunctionsReached (shared): the feature's statements; "…" marks code left out
  private noteJunctionsReached(): void {
    // … 17 lines
    for (const node of this.signalised) if (!this.graph.junctions.has(node) && !noted.has(node)) { noted.add(node); this.noteSignal(node); }
  }

// src/sim/world.ts:863-867 slice of src/sim/world.ts#SimWorld.noteJunctionsReached.note (shared): the feature's statements; "…" marks code left out
    const note = (node: NodeId): void => {
      if (noted.has(node) || !(this.signalised.has(node) || this.graph.junctions.get(node)?.signalised)) return;
    // … 2 lines
    };

// src/sim/world.ts:881-886 slice of src/sim/world.ts#SimWorld.forgetSignals (shared): the feature's statements; "…" marks code left out
  private forgetSignals(): void {
    // … 3 lines
    for (const [node, junction] of this.graph.junctions) if (junction.signalised) this.signalised.add(node);
  }

// src/sim/world.ts:889-906 slice of src/sim/world.ts#SimWorld.applyAutoControl (shared): the feature's statements; "…" marks code left out
  private applyAutoControl(only?: Iterable<NodeId>): boolean {
    // … 2 lines
    const asked = only ? [...only].map((node) => [node, this.graph.junctions.get(node)] as const).filter((e): e is readonly [NodeId, JunctionTopology] => e[1] !== undefined) : this.graph.junctions;
    for (const [node, junction] of asked) {
      rebuildWork.lanes++;
      if (this.doc.node(node)?.control !== 'auto') continue;
      const choice = this.advisor.choice(node);
      if (!choice) continue;
      const signalised = choice === 'signal';
      if (junction.signalised === signalised) continue;
      this.graph.junctions.set(node, { ...junction, signalised });
      this.junctionsChanged([node]);
      this.noteSignal(node);
      changed = true;
    }
    return changed;
  }

// src/sim/world.ts:909-913 slice of src/sim/world.ts#SimWorld.countsAt (shared): the feature's statements; "…" marks code left out
  private countsAt(node: NodeId): boolean {
    if (!this.meso) return this.conflicts.inWindowOf(node);
    // … 1 lines
    return !!n && this.meso.insideWindow(n.x, n.y);
  }

// src/sim/world.ts:919-944 slice of src/sim/world.ts#SimWorld.stepControlAdvisor (shared): the feature's statements; "…" marks code left out
  stepControlAdvisor(dt: number): void {
    this.flow.step(dt);
    this.flow.takeCounted(this.advised);
    if (!this.advisor.tick(dt)) return;
    // … 1 lines
    for (const node of this.advised) {
      const junction = this.graph.junctions.get(node);
      // … 1 lines
      if (!junction || !n || n.control !== 'auto' || n.incident.length < 3) { this.advised.delete(node); continue; }
      // … 1 lines
      if (this.microWindow && !this.countsAt(node)) { this.flow.hold(node); continue; }
      this.flow.resume(node);
      const main = mainRoadLegs(this.doc, node);
      // … 1 lines
      for (const seg of n.incident) {
        const rate = this.flow.rate(node, seg);
        if (main?.includes(seg)) major += rate;
        else { minorMax = Math.max(minorMax, rate); minorTotal += rate; }
      }
      const initial: AutoControl = junction.signalised ? 'signal' : 'priority';
      if (this.advisor.evaluate(node, major, minorMax, minorTotal, initial, this.flow.measured)) changed.add(node);
      // … 1 lines
      if (this.advisor.choice(node) === 'priority' && major + minorTotal < QUIET_FLOW) this.advised.delete(node);
    }
  // … 1 lines
  }

// src/sim/world.ts:947-961 slice of src/sim/world.ts#SimWorld.rebuildWalkTopology (shared): the feature's statements; "…" marks code left out
  rebuildWalkTopology(): void {
    // … 2 lines
    const changes = this.topologyLog.since(this.walkSeen);
    this.walkSeen = this.topologyLog.state;
    // … 1 lines
    if (changes) {
      nodes = new Set();
      for (const change of changes) for (const node of change.junctions) nodes.add(node);
    }
    this.sidewalks.update(this.doc, this.net, this.graph, changes);
    this.crossingSpans.update(this, nodes);
    this.syncControllers(nodes);
  // … 1 lines
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
      // … 1 lines
      connectorsOf: (id: string) => this.graph.connectors.get(id),
      // … 1 lines
      approachSpeed: (lane: string) => this.graph.lanelets.get(lane)?.speedLimit ?? 0,
      // … 1 lines
      conflictsKnown: (node: NodeId) => this.conflicts.holds(node),
      conflictsOf: (id: string) => this.conflicts.refs(id)
        .filter((ref) => this.conflicts.points[ref.point]?.zone(id, CAR_CLASS, CAR_CLASS))
        .map((ref) => ref.other),
    // … 18 lines
    };
  // … 1 lines
  }

// src/sim/world.ts:1000-1023 slice of src/sim/world.ts#SimWorld.makeSignalDeps.reservedGroups (shared): the feature's statements; "…" marks code left out
    const reservedGroups = (): Map<NodeId, Set<number>> => {
      // … 2 lines
      for (const vehicle of this.junctionHolders()) {
        const lane = this.lanelet(vehicle.lanelet);
        const ownsAllocation =
          vehicle.admittedConnector !== null ||
          vehicle.clearingConnectors.length > 0 ||
          lane?.kind === 'connector';
        if (!ownsAllocation) continue;
        // … 1 lines
        const next = vehicle.reservedConnectors[0];
        if (!next) continue;
        const connector = this.connector(next);
        if (!connector) continue;
        let groups = reserved.get(connector.node);
        if (!groups) {
          groups = new Set();
          reserved.set(connector.node, groups);
        }
        groups.add(connector.group);
      }
    // … 1 lines
    };

// src/sim/world.ts:1057-1087 slice of src/sim/world.ts#SimWorld.signalDemand (shared): the feature's statements; "…" marks code left out
  signalDemand(node: NodeId, groups: readonly number[], movements?: readonly string[]): { active: number; score: number } {
    const junction = this.graph.junctions.get(node);
    // … 2 lines
    for (const laneId of junction?.inbound ?? []) {
      // An empty approach serves nobody: read before its group is looked up.
      const order = this.runtime.get(laneId)?.order;
      if (!order?.length) continue;
      const lane = this.graph.lanelets.get(laneId);
      const segment = lane?.segment;
      if (!lane || segment === undefined) continue;
      const group = junction?.groups.find((g) => g.segments.includes(segment));
      if (!group || !groups.includes(group.id)) continue;
      for (let i = order.length - 1; i >= 0; i--) {
        const v = this.vehicles.get(order[i] as number);
        if (!v) continue;
        const d = lane.length - v.s;
        const arrival = d / Math.max(v.v, 0.5);
        if (d > DEMAND_QUEUE_REACH && arrival > DEMAND_HORIZON) break;
        const next = this.graph.connectors.get(v.route[1] ?? '');
        if (!next || next.fromLane !== laneId) continue;
        if (movements && !movements.includes(next.id)) continue;
        if (i === order.length - 1) {
          if (!hasDownstreamStorage(this, v, next)) break;
          if (d <= DEMAND_AT_LINE || arrival <= DEMAND_PASSAGE) active++;
        }
        score += v.v < 0.5 ? 1 + v.waited / DEMAND_WAIT_WEIGHT : Math.max(0, 1 - arrival / DEMAND_HORIZON);
      }
    }
    return { active, score };
  }

// src/sim/world.ts:1090-1101 slice of src/sim/world.ts#SimWorld.junctionQuiet (shared): the feature's statements; "…" marks code left out
  private junctionQuiet(c: SignalController, reserved: () => ReadonlyMap<NodeId, ReadonlySet<number>>): boolean {
    const junction = this.graph.junctions.get(c.node);
    if (!junction) return false;
    for (const lane of junction.inbound) if ((this.runtime.get(lane)?.order.length ?? 0) > 0) return false;
    for (const stage of c.plan.stages) {
      for (const x of stage.pedWalk) {
        const at = this.crossingStates.get(x);
        if (at && (at.occupants.length > 0 || at.demand || at.waitingFrom > 0 || at.waitingTo > 0)) return false;
      }
    }
    return !reserved().has(c.node);
  }

// src/sim/world.ts:1104-1119 slice of src/sim/world.ts#SimWorld.busNear (shared): the feature's statements; "…" marks code left out
  busNear(node: NodeId, groups: readonly number[]): boolean {
    for (const laneId of this.graph.junctions.get(node)?.inbound ?? []) {
      const order = this.runtime.get(laneId)?.order;
      if (!order?.length) continue;
      const lane = this.lanelet(laneId);
      if (!lane || lane.kind !== 'link' || lane.to !== node) continue;
      for (const id of order) {
        const v = this.vehicles.get(id);
        if (!v || v.archetype.shape !== 'bus' || lane.length - v.s > BUS_REACH) continue;
        const next = v.route[v.route.indexOf(v.lanelet) + 1];
        const conn = next ? this.connector(next) : undefined;
        if (conn && conn.node === node && groups.includes(conn.group)) return true;
      }
    }
  // … 1 lines
  }

// src/sim/world.ts:1127-1164 slice of src/sim/world.ts#SimWorld.syncControllers (shared): the feature's statements; "…" marks code left out
  private syncControllers(only: ReadonlySet<NodeId> | null = null): void {
    // … 2 lines
    const asked = only ? [...only].map((node) => [node, this.graph.junctions.get(node)] as const).filter((e): e is readonly [NodeId, JunctionTopology] => e[1] !== undefined) : this.graph.junctions;
    for (const [node, junction] of asked) {
      const crossings = this.crossingsAt(node);
      const existing = this.controllers.get(node);
      const settings = this.doc.node(node)?.signal;
      const inputs = this.planInputsOf(junction, crossings, settings);
      if (existing) {
        const was = this.planInputs.get(node);
        if (was && samePlanInputs(was, inputs)) continue;
        rebuildWork.signals++;
        rebuildController(existing, junction, crossings, deps);
        this.noteSignal(node);
        // A fixed-time signal back where the common clock says, with its offset (V5).
        if (settings?.mode === 'fixed') seekFixed(existing, settings, this.tickCount() * DT);
      } else {
        rebuildWork.signals++;
        // A deterministic per-node offset so neighbouring junctions do not all switch together.
        const offset = (node * 7.317) % 60;
        const created = createController(junction, crossings, deps, offset, this.signalClock);
        if (settings?.mode === 'fixed') seekFixed(created, settings, this.tickCount() * DT);
        this.controllers.set(node, created);
        this.noteSignal(node);
      }
      this.planInputs.set(node, inputs);
    }
    // … 1 lines
    for (const node of only ? [...only] : [...this.controllers.keys()]) {
      if (this.controllers.has(node) && !this.graph.junctions.has(node)) {
        this.controllers.delete(node);
        this.planInputs.delete(node);
        this.noteSignal(node);
      }
    }
  // … 1 lines
  }

// src/sim/world.ts:1170-1179 slice of src/sim/world.ts#SimWorld.planInputsOf (shared): the feature's statements; "…" marks code left out
  private planInputsOf(junction: JunctionTopology, crossings: readonly CrossingId[], settings: SignalSettings | undefined): PlanInputs {
    return {
      groups: junction.groups,
      connectors: junction.connectors.map((id) => this.graph.connectors.get(id)),
      signalised: junction.signalised,
      conflicts: this.conflicts.holds(junction.node),
      crossings: crossings.join(','),
      settings: settingsKey(settings),
    };
  }

// src/sim/world.ts:1184-1202 slice of src/sim/world.ts#SimWorld.enterLanelet (shared): the feature's statements; "…" marks code left out
  enterLanelet(v: Vehicle, id: LaneletId, recordVolume = true): void {
    const rt = this.runtimeFor(id);
    if (!rt.order.includes(v.id)) rt.order.push(v.id);
    this.occupied.add(id);
    this.sortLane(rt);
    v.lanelet = id;
    const lane = this.lanelet(id);
    if (lane?.kind === 'connector') this.noteHolder(v);
    // … 1 lines
    if (lane?.kind !== 'link' || !lane.controlled) { v.greenStall = 0; v.greenDenied = 0; }
    // … 1 lines
    if (recordVolume && lane?.kind === 'connector' && lane.node !== undefined) {
      const from = this.lanelet(lane.fromLane ?? '')?.segment;
      if (from !== undefined) this.flow.record(lane.node, from);
    }
    if (recordVolume && lane?.kind === 'link' && lane.segment !== undefined) {
      this.segmentVolume.set(lane.segment, (this.segmentVolume.get(lane.segment) ?? 0) + 1);
    }
  }

// src/sim/world.ts:1204-1210 slice of src/sim/world.ts#SimWorld.exitLanelet (shared): the feature's statements; "…" marks code left out
  exitLanelet(v: Vehicle, id: LaneletId): void {
    const rt = this.runtime.get(id);
    if (!rt) return;
    const i = rt.order.indexOf(v.id);
    if (i >= 0) rt.order.splice(i, 1);
    if (rt.order.length === 0) this.occupied.delete(id);
  }

// src/sim/world.ts:1213-1227 slice of src/sim/world.ts#SimWorld.sortLane (shared): the feature's statements; "…" marks code left out
  sortLane(rt: LaneletRuntime): void {
    // … 1 lines
    for (let i = 1; i < order.length; i++) {
      // … 1 lines
      const s = this.vehicles.get(id)?.s ?? 0;
      // … 1 lines
      while (j >= 0) {
        // … 1 lines
        if (((this.vehicles.get(other)?.s ?? 0) - s || other - id) <= 0) break;
      // … 2 lines
      }
    // … 1 lines
    }
  }

// src/sim/world.ts:1230-1234 method SimWorld.laneHead: the feature's own
  laneHead(id: LaneletId): Vehicle | undefined {
    const order = this.rt(id).order;
    const last = order[order.length - 1];
    return last === undefined ? undefined : this.vehicles.get(last);
  }

// src/sim/world.ts:1243-1248 method SimWorld.addShadow: the feature's own
  addShadow(v: Vehicle, lanelet: LaneletId, offset: number, clearAt: number): void {
    this.clearShadow(v);
    v.shadow = { lanelet, offset, clearAt };
    const rt = this.runtimeFor(lanelet);
    if (!rt.shadows.includes(v.id)) rt.shadows.push(v.id);
  }

// src/sim/world.ts:1250-1258 slice of src/sim/world.ts#SimWorld.clearShadow (shared): the feature's statements; "…" marks code left out
  clearShadow(v: Vehicle): void {
    if (!v.shadow) return;
    const rt = this.runtime.get(v.shadow.lanelet);
    if (rt) {
      const i = rt.shadows.indexOf(v.id);
      if (i >= 0) rt.shadows.splice(i, 1);
    }
    v.shadow = null;
  }

// src/sim/world.ts:1261-1284 slice of src/sim/world.ts#SimWorld.bodiesIn (shared): the feature's statements; "…" marks code left out
  bodiesIn(id: LaneletId): { vehicle: Vehicle; s: number }[] {
    const rt = this.rt(id);
    const out: { vehicle: Vehicle; s: number }[] = [];
    for (const vid of rt.order) {
      const v = this.vehicles.get(vid);
      if (v) out.push({ vehicle: v, s: v.s });
    }
    for (const vid of rt.shadows) {
      const v = this.vehicles.get(vid);
      if (v?.shadow?.lanelet === id) out.push({ vehicle: v, s: v.s + v.shadow.offset });
    }
    const lane = this.lanelet(id);
    if (lane) {
      for (const next of this.graph.exitsOf(id)) {
        for (const vid of this.rt(next).order) {
          const v = this.vehicles.get(vid);
          if (v && v.rearPath[0] === id && v.s < v.archetype.length) {
            out.push({ vehicle: v, s: lane.length + v.s });
          }
        }
      }
    }
    return out;
  }

// src/sim/world.ts:1286-1292 slice of src/sim/world.ts#SimWorld.removeVehicle (shared): the feature's statements; "…" marks code left out
  removeVehicle(v: Vehicle): void {
    this.maybeHolders.delete(v);
    this.clearShadow(v);
    this.exitLanelet(v, v.lanelet);
    this.claims.releaseAll(v.id);
    this.vehicles.delete(v.id);
  }

// src/sim/world.ts:1311-1311 function crossingNode: the feature's own
const crossingNode = (id: CrossingId): NodeId => Number(id.slice(0, id.indexOf(':'))) as NodeId;

// src/sim/world.ts:1314-1322 slice of src/sim/world.ts#PlanInputs (shared): the feature's statements; "…" marks code left out
interface PlanInputs {
  readonly groups: JunctionTopology['groups'];
  readonly connectors: readonly (Connector | undefined)[];
// … 5 lines
}

// src/sim/world.ts:1325-1328 slice of src/sim/world.ts#settingsKey (shared): the feature's statements; "…" marks code left out
function settingsKey(s: SignalSettings | undefined): string {
  if (!s) return '';
  return `${s.mode ?? '-'}|${s.greens?.join(',') ?? '-'}|${s.offset ?? '-'}|${s.busPriority ? 1 : 0}`;
}
