// What the feature uses of src/sim/world.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/world.ts:31-31 variable BUS_REACH
/** How far before its line a bus asks for priority, world units (V5). */
const BUS_REACH = m(90);

// src/sim/world.ts:39-39 variable BODY_OFF_LANE
/** How far a vehicle's pose can lie outside its lane's bounds (`vehiclesNear`): the longest body (its rear axle still on the lane behind) and a lane change's sideways reach. */
const BODY_OFF_LANE = m(20);

// src/sim/world.ts:41-41 variable WINDOW_EVERY
/** Seconds of the city's time between moves of the window. */
const WINDOW_EVERY = 0.5;

// src/sim/world.ts:43-43 variable JUNCTION_FILED
/** What the filing holds for a junction: its id is the whole of it. */
const JUNCTION_FILED = {};

// src/sim/world.ts:46-46 type WorldSignalDeps
/** What the signals read of the world (`SignalDeps`), and the reservations by node, which wake a sleeping one (`SimWorld.signalsToStep`). */
export type WorldSignalDeps = SignalDeps & { readonly reserved: () => ReadonlyMap<NodeId, ReadonlySet<number>> };

// src/sim/world.ts:58-58 variable CAR_CLASS
/** The body class a signal plan is protected for: an ordinary car. */const CAR_CLASS: BodyClass = 1;

// src/sim/world.ts:61-65 variable NO_PEDESTRIANS
/** Nobody walks until an engine is installed (`SimWorld.usePedestrianEngine`). */
const NO_PEDESTRIANS: PedestrianEngine = {
  kind: 'people',
  beginTick() {}, dispatch() {}, step() {}, rebind() {}, publish() {}, audit() {}, reset() {},
  bridge: { hailable: () => null, board: () => null, alight: () => false, anyoneWithin: () => false },
};

// src/sim/world.ts:68-68 variable DEMAND_QUEUE_REACH
/** A queue is counted this far back from the stop line. */
const DEMAND_QUEUE_REACH = m(80);

// src/sim/world.ts:70-70 variable DEMAND_HORIZON
/** Moving vehicles are counted when they will arrive within this, seconds. */
const DEMAND_HORIZON = 12;

// src/sim/world.ts:72-72 variable DEMAND_AT_LINE
/** A head this close, or arriving within the passage time, keeps a green alive. */
const DEMAND_AT_LINE = m(8);

// src/sim/world.ts:73-73 variable DEMAND_PASSAGE
const DEMAND_PASSAGE = 3;

// src/sim/world.ts:75-75 variable DEMAND_WAIT_WEIGHT
/** Seconds of waiting that double a queued vehicle's weight. */
const DEMAND_WAIT_WEIGHT = 30;

// src/sim/world.ts:78-88 interface LaneletRuntime
/** Vehicles occupying one lanelet, kept sorted by ascending arc position. */
export interface LaneletRuntime {
  readonly id: LaneletId;
  /** Ascending by `s`; index 0 is nearest the entry, last is nearest the exit. */
  order: VehicleId[];
  /** Vehicles whose body still overlaps this lane while they slide out of it. */
  shadows: VehicleId[];
  /** A retired lanelet still carrying agents: no new entries accepted. */
  ghost: boolean;
  /** Tick at which a ghost was created, so it can be force-collected. */
  ghostSince: number;
}

// src/sim/world.ts:91-109 interface SimStreams
/** The world's random streams, one per consumer, forked by tag so a new consumer reshuffles no other (`core/rng.ts`). */
export interface SimStreams {
  readonly spawnVehicles: Rng;
  readonly spawnPeds: Rng;
  readonly driver: Rng;
  readonly route: Rng;
  readonly gap: Rng;
  readonly pedParams: Rng;
  readonly courtesy: Rng;
  readonly signalOffsets: Rng;
  /** The People engine's own stream (`people/people.ts`). */
  readonly people: Rng;
  readonly occupancy: Rng;
  /** The lots' cars (`agents/lotTraffic.ts`), the lorries (`agents/lorryTraffic.ts`) and the scenery's life (`ambient/ambient.ts`). */
  readonly lots: Rng;
  readonly lorries: Rng;
  readonly ambient: Rng;
  /** The meso traffic's cars made in the micro window (`meso/meso.ts`). */
  readonly meso: Rng;
}

// src/sim/world.ts:134-140 variable NO_RUNTIME
/** What `SimWorld.rt` answers for a lane nobody has been on: empty, not a ghost, frozen against writes. */
const NO_RUNTIME: LaneletRuntime = Object.freeze({
  id: '' as LaneletId,
  order: Object.freeze([]) as unknown as VehicleId[],
  shadows: Object.freeze([]) as unknown as VehicleId[],
  ghost: false,
  ghostSince: 0,
});

// src/sim/world.ts:144-151 interface MesoTraffic
/** Everything the simulation owns. */
/** The meso traffic as the world steps it (`meso/meso.ts` Meso): declared here so the world never imports the meso, which reads the world. */
export interface MesoTraffic {
  step(w: SimWorld, traffic: boolean): void;
  reset(): void;
  /** Whether a point is inside its micro window (the flows at a junction are counted there). */
  insideWindow(x: number, y: number): boolean;
  /** The micro window as it stands (null before it stands anywhere): the parked cars stand only inside it (`ambient.ts`). */
  microWindow(): { readonly x: number; readonly y: number; readonly r: number } | null;
}

// src/sim/world.ts:153-1302 class SimWorld
export class SimWorld { … }

// src/sim/world.ts:154-154 field SimWorld.clock
readonly clock = new SimClock();

// src/sim/world.ts:155-155 field SimWorld.graph
readonly graph = new LaneletGraph();

// src/sim/world.ts:156-156 field SimWorld.conflicts
readonly conflicts = new ConflictIndex();

// src/sim/world.ts:157-157 field SimWorld.sidewalks
readonly sidewalks = new SidewalkGraph();

// src/sim/world.ts:159-159 field SimWorld.signalPosts
/** The traffic signal posts by place, kept from the junctions' controls (`signalChangesSince`): the walkers keep clear of them. */
  readonly signalPosts = new SignalPostIndex();

// src/sim/world.ts:160-160 field SimWorld.claims
readonly claims = new ClaimTable();

// src/sim/world.ts:163-167 field SimWorld.vehicles
/** @mapWide Every vehicle on the map; each filed by its route as it comes and goes (`routes`). */
  readonly vehicles = new VersionedMap<VehicleId, Vehicle>({
    set: (v, was) => { if (was && was !== v) this.routes.unfile(was); this.routes.file(v); },
    delete: (v) => this.routes.unfile(v),
    clear: () => this.routes.clear(),
  });

// src/sim/world.ts:169-169 field SimWorld.routes
/** The vehicles by the lanes and movements of their routes (`noteRoute`): those an edit reached (`vehiclesReached`). */
  readonly routes = new RouteIndex<Vehicle>();

// src/sim/world.ts:171-171 field SimWorld.rebindLanes
/** The lanes and movements the builds since the last rebind took or made again (`vehiclesReached`); `rebindAll` after a whole build. */
  private readonly rebindLanes = new Set<LaneletId>();

// src/sim/world.ts:182-182 field SimWorld.pedEngine
/** What moves the people (`people/engine.ts`); everything else reaches them through it. */
  pedEngine: PedestrianEngine = NO_PEDESTRIANS;

// src/sim/world.ts:186-186 field SimWorld.runtime
/** @mapWide One per lanelet a vehicle (or its shadow) has been on; written only through `runtimeFor`. */
  readonly runtime = new Map<LaneletId, LaneletRuntime>();

// src/sim/world.ts:187-187 field SimWorld.controllers
readonly controllers = new Map<NodeId, SignalController>();

// src/sim/world.ts:189-189 field SimWorld.doLess
/** DOING LESS (phase 1) */
  doLess = true;

// src/sim/world.ts:191-191 field SimWorld.occupied
/** Lanes with a vehicle on them: written only by `enterLanelet`/`exitLanelet`, the only writers of `order`. */
  readonly occupied = new Set<LaneletId>();

// src/sim/world.ts:195-195 field SimWorld.maybeHolders
/** DO NOT find them by walking the fleet: three passes over every vehicle of the map each tick */
  private readonly maybeHolders = new Set<Vehicle>();

// src/sim/world.ts:197-197 field SimWorld.ghostCandidates
/** Runtime lanes no longer in the graph (an edit took them): stage 7's work list (`collectGhosts`). */
  readonly ghostCandidates = new Set<LaneletId>();

// src/sim/world.ts:199-199 field SimWorld.asleep
/** Signals asleep (`signalsToStep`), by node: rested in green with nobody near on their last step. */
  private readonly asleep = new Map<NodeId, SignalController>();

// src/sim/world.ts:201-201 field SimWorld.signalClock
/** The passes of stage 1, one a tick (`pipeline.ts`): the clock every controller's time is read against (`SignalClock`). */
  readonly signalClock: SignalClock = { passes: 0 };

// src/sim/world.ts:204-204 field SimWorld.crossingStates
/** @mapWide What vehicles, signals and the audit may know about the people at each crossing (`crossings/state.ts`). */
  readonly crossingStates: CrossingStates = new Map();

// src/sim/world.ts:206-206 field SimWorld.pedViews
/** @mapWide Every person as the renderer and the counters see them */
  readonly pedViews: PedView[] = [];

// src/sim/world.ts:208-208 field SimWorld.pedViewById
/** @mapWide Every person on the map, by id. */
  readonly pedViewById = new Map<number, PedView>();

// src/sim/world.ts:210-210 field SimWorld.crossingSpans
/** Stretch of each zebra that each movement drives over. */
  readonly crossingSpans = new CrossingSpans();

// src/sim/world.ts:213-213 field SimWorld.lastAdmission
/** Tick at which each junction last admitted a vehicle. */
  readonly lastAdmission = new Map<NodeId, number>();

// src/sim/world.ts:215-215 field SimWorld.mergeTurn
/** The lane that last sent a vehicle into each lane two or more lanes merge into, for the zipper */
  readonly mergeTurn = new Map<LaneletId, LaneletId>();

// src/sim/world.ts:219-219 getter SimWorld.flow
/** Vehicles an hour into each junction from each leg, as a trend. */
  get flow(): FlowStats;

// src/sim/world.ts:220-220 field SimWorld.flowStats
private flowStats = new FlowStats();

// src/sim/world.ts:222-222 getter SimWorld.advisor
/** The control each junction left on "automatic" gets from its flows. */
  get advisor(): ControlAdvisor;

// src/sim/world.ts:223-223 field SimWorld.controlAdvisor
private controlAdvisor = new ControlAdvisor();

// src/sim/world.ts:226-226 getter SimWorld.rng
/** The world's random streams, forked from its seed (`reset` forks them again). */
  get rng(): SimStreams;

// src/sim/world.ts:229-229 field SimWorld.nextVehicleId
nextVehicleId = 1;

// src/sim/world.ts:237-237 field SimWorld.laneReaches
/** The road ends each lane reaches, per body class, kept by what each edit changed (`routing/laneReach.ts`). */
  readonly laneReaches = new LaneReaches();

// src/sim/world.ts:239-239 field SimWorld.vehicleSpawnClock
/** Per-world population timer; simulations must never influence each other. */
  vehicleSpawnClock = 0;

// src/sim/world.ts:242-242 field SimWorld.entryDemand
/** Traffic waiting to come in at each boundary entry lane */
  /** Per entry: the next arrival, the rate it was drawn at, and the arrivals waiting outside. */
  readonly entryDemand = new Map<LaneletId, { next: number; rate: number; waiting: number[] }>();

// src/sim/world.ts:244-244 field SimWorld.entryDemandLost
/** Arrivals turned away because an entry's outside queue was full. */
  entryDemandLost = 0;

// src/sim/world.ts:246-246 field SimWorld.completedTrips
/** Vehicles that reached the boundary exit chosen for their trip. */
  completedTrips = 0;

// src/sim/world.ts:248-248 field SimWorld.trafficIntensity
/** User-facing density multipliers; topology and physics remain unchanged. */
  trafficIntensity = 1;

// src/sim/world.ts:251-251 field SimWorld.trafficCount
/** How many cars and how many people on foot the player wants on the map (the panel's Traffic and People, `main.ts`). */
  trafficCount: number | null = null;

// src/sim/world.ts:254-254 field SimWorld.followCity
/** The game's way (`main.ts`): the panel's Traffic and People are shares of the city's own density */
  followCity = false;

// src/sim/world.ts:256-256 field SimWorld.demandMultiplier
/** Shared demand multiplier for the current simulation period. */
  demandMultiplier = 1;

// src/sim/world.ts:258-258 field SimWorld.edgeTraffic
/** Traffic from outside: cars at the map's edge entries, people at doors and road ends (set by `City.step`). */
  edgeTraffic = true;

// src/sim/world.ts:260-260 field SimWorld.city
/** The city's services - the time of day, the public transport (`sim/city/city.ts`) - stepped by the pipeline. */
  readonly city = new City();

// src/sim/world.ts:262-265 field SimWorld.reservationsChecked
/** Each vehicle's reservation chain as last found valid (`pipeline.ts` reconcileReservedChain): walked again only when what it reads moved. */
  readonly reservationsChecked = new WeakMap<Vehicle, {
    readonly lanelet: LaneletId; readonly admitted: LaneletId | null; readonly reserved: readonly LaneletId[]; readonly reservedLength: number;
    readonly route: readonly LaneletId[]; readonly routeLength: number; readonly topology: object;
  }>();

// src/sim/world.ts:267-267 field SimWorld.ambient
/** The life of the scenery: people and traffic made round the view (`sim/ambient`). */
  readonly ambient = new AmbientWorld();

// src/sim/world.ts:269-269 field SimWorld.meso
/** The meso traffic of the whole map and its micro window (`meso/meso.ts`): the game's (`simHost.ts`); off in the specs, the micro simulation everywhere. */
  meso: MesoTraffic | null = null;

// src/sim/world.ts:271-271 field SimWorld.populationShare
/** Share of the population ceilings this device carries (1, or `NARROW_SCREEN_SHARE` on a narrow screen). */
  populationShare = 1;

// src/sim/world.ts:273-277 field SimWorld.focus
/** Where the player is looking, world units */
  focus: {
    readonly x: number; readonly y: number; readonly r: number; readonly detail: boolean;
    /** Playing (`play.ts`), the camera looks along the ground from behind the player and sees far down the street, past `r` */
    readonly view?: { readonly ex: number; readonly ey: number; readonly dx: number; readonly dy: number; readonly cos: number; readonly far: number } | null;
  } | null = null;

// src/sim/world.ts:286-286 field SimWorld.topologyRevision
/** Topology revision the indices were last built from. */
  topologyRevision = -1;

// src/sim/world.ts:293-301 constructor SimWorld.constructor
constructor(
    readonly doc: RoadDoc,
    readonly net: Network,
    private readonly seed = 0x5eed,
    /** The buildings as traffic reads them: the document's own (a spec), or the worker's table of traffic records (`simHost.ts`). */
    readonly buildings: TrafficBuildings = docBuildings(doc),
  );

// src/sim/world.ts:313-357 method SimWorld.reset
/** Forgets every agent and everything learned about the previous map. */
  reset(): void;

// src/sim/world.ts:361-363 method SimWorld.lanelet
lanelet(id: LaneletId): Lanelet | undefined;

// src/sim/world.ts:371-373 method SimWorld.connector
connector(id: string): Connector | undefined;

// src/sim/world.ts:376-378 method SimWorld.rt
/** DO NOT create the runtime here: routing, lane changes and the inspector read every lane they look at through this */
  rt(id: LaneletId): LaneletRuntime;

// src/sim/world.ts:381-389 method SimWorld.runtimeFor
/** A lane's runtime, made when a vehicle or its shadow first comes onto it: for `enterLanelet` and `addShadow` only. */
  private runtimeFor(id: LaneletId): LaneletRuntime;

// src/sim/world.ts:391-393 method SimWorld.veh
veh(id: VehicleId): Vehicle | undefined;

// src/sim/world.ts:395-397 method SimWorld.controller
controller(node: NodeId): SignalController | undefined;

// src/sim/world.ts:400-402 method SimWorld.noteRoute
/** A vehicle's route written (`Vehicle.route`): for its writers, so an edit finds who drives over what it took (`routes`). */
  noteRoute(v: Vehicle): void;

// src/sim/world.ts:405-426 method SimWorld.vehiclesReached
/** The vehicles the builds since the last call reached, in id order (null: every one, a whole build) */
  vehiclesReached(): readonly Vehicle[] | null;

// src/sim/world.ts:429-431 method SimWorld.noteHolder
/** A vehicle that may now hold something at a junction (`maybeHolders`): for the writers of what it holds. */
  noteHolder(v: Vehicle): void;

// src/sim/world.ts:434-442 method SimWorld.junctionHolders
/** The vehicles holding or clearing a movement, or on a connector, now - in no set order */
  junctionHolders(): Vehicle[];

// src/sim/world.ts:481-483 method SimWorld.vehiclesInIdOrder
/** DO NOT sort the fleet again at each change of it */
  vehiclesInIdOrder(): readonly Vehicle[];

// src/sim/world.ts:486-488 method SimWorld.junctionNodesInOrder
/** Every junction in node order: for the readers of the whole map (doing everything, a whole layout), sorted when asked after a change. */
  junctionNodesInOrder(): readonly NodeId[];

// src/sim/world.ts:491-493 method SimWorld.signalisedControllers
/** The controllers of the signalised junctions in node order: every one, for doing everything and the specs. */
  signalisedControllers(): readonly SignalController[];

// src/sim/world.ts:496-496 field SimWorld.ordered
/** DO NOT sort and map every junction of the map for an edit: kept null by `junctionsChanged` until a reader of every junction asks. */
  private ordered: { readonly nodes: readonly NodeId[]; readonly signalised: readonly SignalController[] } | null = null;

// src/sim/world.ts:498-507 method SimWorld.inNodeOrder
/** @rebuild The junctions and signals in node order, made again after a change when a reader of every one asks. */
  private inNodeOrder(): { readonly nodes: readonly NodeId[]; readonly signalised: readonly SignalController[] };

// src/sim/world.ts:519-523 method SimWorld.junctionOfLane
/** The junction a lane is an approach of (in its `inbound`), from the graph: no map of every approach of the map to keep. */
  private junctionOfLane(id: LaneletId): NodeId | undefined;

// src/sim/world.ts:546-581 method SimWorld.signalsToStep
/** DO NOT walk every signal of the map a tick to pass over those asleep: their time keeps itself (`SignalClock`), only the awake are stepped */
  signalsToStep(deps: WorldSignalDeps): readonly SignalController[];

// src/sim/world.ts:582-582 field SimWorld.stepping
private readonly stepping: SignalController[] = [];

// src/sim/world.ts:585-585 field SimWorld.awake
/** The signalised controllers not asleep: those stage 1 steps (`signalsToStep`). */
  private readonly awake = new Map<NodeId, SignalController>();

// src/sim/world.ts:588-592 method SimWorld.wakeEverySignal
/** @rebuild Every signalised controller awake: a whole build, or changes the diary cannot place. */
  private wakeEverySignal(): void;

// src/sim/world.ts:601-606 method SimWorld.wakeSignal
private wakeSignal(node: NodeId): void;

// src/sim/world.ts:612-614 method SimWorld.noteSignalStepped
/** A signal stepped by stage 1 (`pipeline.ts`). */
  noteSignalStepped(c: SignalController): void;

// src/sim/world.ts:625-629 method SimWorld.restSignal
/** A signal rested with nobody near (`stepController`): not stepped until something wakes it. */
  restSignal(c: SignalController): void;

// src/sim/world.ts:641-641 field SimWorld.prepGraph
/** The first half of `rebuildTopology`: lanelets, conflict zones, claims and the signal controllers */
  /** A second lanelet graph, built from the same document, that is never run */
  private prepGraph: LaneletGraph | null = null;

// src/sim/world.ts:644-672 method SimWorld.prepareVehicleTopology
/** @rebuild The slow half of `rebuildVehicleTopology` ahead of time, in steps */
  *prepareVehicleTopology(measurer?: PairMeasurer): Generator<void | typeof WAITING, void>;

// src/sim/world.ts:676-680 method SimWorld.prepReach
private prepReach(changed: GraphChange | null): ReadonlySet<NodeId> | null;

// src/sim/world.ts:685-687 method SimWorld.walkwaysNow
/** The walkways of the network as it is now. */
  walkwaysNow(): WalkGraph;

// src/sim/world.ts:700-700 field SimWorld.topologyLog
/** What each build of the lane graph changed */
  readonly topologyLog = new ChangeLog<GraphChange>();

// src/sim/world.ts:705-713 method SimWorld.fileJunctions
/** The junctions a build changed filed again by place; every one for a graph built whole. */
  private fileJunctions(changed: GraphChange | null): void;

// src/sim/world.ts:716-720 method SimWorld.useMicroWindow
/** The game's simulation keeps conflicts only where traffic is: round the focus and every vehicle (DO NOT build every junction's pairs of the map: 1.3 KB each, 63 MB in a 4,868-building city). */
  useMicroWindow(): void;

// src/sim/world.ts:723-731 method SimWorld.moveWindowSteps
/** The window moved to the one asked for: the pairs of the junctions it reaches measured off the thread first (`measurer`, `conflictPool.ts`), then the swap (`TopologyCatchUp.stepWindow`). */
  *moveWindowSteps(measurer?: PairMeasurer): Generator<void | typeof WAITING, void>;

// src/sim/world.ts:734-739 method SimWorld.windowMoved
/** The junctions whose conflict pairs came into the index or left it: claims on pairs gone dropped, and their signals planned again, now from the pairs or from the approaches' geometry (`planInputsOf`). */
  private windowMoved(changed: ReadonlySet<NodeId>): void;

// src/sim/world.ts:755-778 method SimWorld.keepMicroWindow
/** Stage 2 (`pipeline.ts`): the window asked for with the focus and the vehicles, each half second or when vehicles were made; moved off the step (`moveWindowSteps`). */
  keepMicroWindow(): void;

// src/sim/world.ts:781-825 method SimWorld.rebuildVehicleTopology
/** @rebuild Called when the network's traffic revision moved. */
  rebuildVehicleTopology(): void;

// src/sim/world.ts:841-852 method SimWorld.noteSignal
private noteSignal(node: NodeId): void;

// src/sim/world.ts:859-878 method SimWorld.noteJunctionsReached
/** The signalised junctions (now, or when last noted) at the ends of the segments the roads' changes reached, noted once each; the log forgotten when the diary cannot say.
   *  DO NOT note every junction a tile's square reaches: a junction with no signal has no heads nor posts, and one that gains or loses its controller is noted where that happens (`syncControllers`, `applyAutoControl`). */
  private noteJunctionsReached(): void;

// src/sim/world.ts:881-886 method SimWorld.forgetSignals
/** @rebuild The log forgotten (readers read every junction): the signalised ones listed again from the graph. */
  private forgetSignals(): void;

// src/sim/world.ts:889-906 method SimWorld.applyAutoControl
/** The control a junction on "automatic" runs */
  private applyAutoControl(only?: Iterable<NodeId>): boolean;

// src/sim/world.ts:909-913 method SimWorld.countsAt
/** Whether the flows at a junction are counted now: inside the window the meso hands its cars to the micro simulation in (`meso.ts`), where vehicles drive through it; the conflicts' window without the meso. */
  private countsAt(node: NodeId): boolean;

// src/sim/world.ts:919-944 method SimWorld.stepControlAdvisor
/** Stage 0's measuring of the junctions: DO NOT evaluate every junction of the map, only those whose control the flows can still change */
  stepControlAdvisor(dt: number): void;

// src/sim/world.ts:947-961 method SimWorld.rebuildWalkTopology
/** The second half: the footway graph and its crossings, and the controllers again */
  rebuildWalkTopology(): void;

// src/sim/world.ts:964-973 method SimWorld.crossingsAt
/** Crossing ids at a node, one per incident segment. */
  crossingsAt(node: NodeId): CrossingId[];

// src/sim/world.ts:983-1054 method SimWorld.makeSignalDeps
private makeSignalDeps(): { readonly deps: WorldSignalDeps; readonly fresh: () => void };

// src/sim/world.ts:1057-1087 method SimWorld.signalDemand
/** What a green for these groups would actually serve right now. */
  signalDemand(node: NodeId, groups: readonly number[], movements?: readonly string[]): { active: number; score: number };

// src/sim/world.ts:1090-1101 method SimWorld.junctionQuiet
/** Nobody near a signal's junction (`SignalDeps.quiet`): its approaches empty, nobody at its crossings, nothing reserved there. */
  private junctionQuiet(c: SignalController, reserved: () => ReadonlyMap<NodeId, ReadonlySet<number>>): boolean;

// src/sim/world.ts:1104-1119 method SimWorld.busNear
/** Whether a bus is on an approach of `node`, within reach of its line, bound for a movement of one of these signal groups */
  busNear(node: NodeId, groups: readonly number[]): boolean;

// src/sim/world.ts:1127-1164 method SimWorld.syncControllers
/** DO NOT rebuild every controller here */
  private syncControllers(only: ReadonlySet<NodeId> | null = null): void;

// src/sim/world.ts:1170-1179 method SimWorld.planInputsOf
/** What a junction's plan is made from (`signals/plan.ts` buildSignalPlan): its approach groups and connectors */
  private planInputsOf(junction: JunctionTopology, crossings: readonly CrossingId[], settings: SignalSettings | undefined): PlanInputs;

// src/sim/world.ts:1184-1202 method SimWorld.enterLanelet
/** Inserts a vehicle into a lanelet's occupancy list, keeping it sorted. */
  enterLanelet(v: Vehicle, id: LaneletId, recordVolume = true): void;

// src/sim/world.ts:1204-1210 method SimWorld.exitLanelet
exitLanelet(v: Vehicle, id: LaneletId): void;

// src/sim/world.ts:1213-1227 method SimWorld.sortLane
/** A lane's queue ordered by arc position, then id (the one order every reader of it assumes). DO NOT sort it whole with a new comparator each step (300-400 lanes a step, SIM-09): an insertion pass with the same key, linear on a queue already in order (nobody overtakes within a lane); the key a total order, so the same order any sort gives. */
  sortLane(rt: LaneletRuntime): void;

// src/sim/world.ts:1250-1258 method SimWorld.clearShadow
clearShadow(v: Vehicle): void;

// src/sim/world.ts:1261-1284 method SimWorld.bodiesIn
/** Every body in a lane with its front arc position there */
  bodiesIn(id: LaneletId): { vehicle: Vehicle; s: number }[];

// src/sim/world.ts:1286-1292 method SimWorld.removeVehicle
removeVehicle(v: Vehicle): void;

// src/sim/world.ts:1314-1322 interface PlanInputs
/** What a controller's plan was made from (`SimWorld.planInputsOf`). */
interface PlanInputs {
  readonly groups: JunctionTopology['groups'];
  readonly connectors: readonly (Connector | undefined)[];
  readonly signalised: boolean;
  /** Whether the conflict index held the junction's pairs (`ConflictIndex.holds`). */
  readonly conflicts: boolean;
  readonly crossings: string;
  readonly settings: string;
}

// src/sim/world.ts:1325-1328 function settingsKey
/** The player's settings of a signal as one string, field by field (`SignalSettings`). */
function settingsKey(s: SignalSettings | undefined): string;

// src/sim/world.ts:1330-1335 function samePlanInputs
function samePlanInputs(a: PlanInputs, b: PlanInputs): boolean;

// src/sim/world.ts:1338-1400 class VersionedMap
/** A Map that counts its changes of membership, for caches over it, and keeps its values in the order of their keys */
class VersionedMap<K extends number, V> extends Map<K, V> { … }

// src/sim/world.ts:1349-1356 method VersionedMap.set
override set(key: K, value: V): this;
