// What the feature uses of src/sim/world.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/world.ts:31-31 variable BUS_REACH
/** How far before its line a bus asks for priority, world units (V5). */
const BUS_REACH = m(90);

// src/sim/world.ts:43-43 variable JUNCTION_FILED
/** What the filing holds for a junction: its id is the whole of it. */
const JUNCTION_FILED = {};

// src/sim/world.ts:46-46 type WorldSignalDeps
/** What the signals read of the world (`SignalDeps`), and the reservations by node, which wake a sleeping one (`SimWorld.signalsToStep`). */
export type WorldSignalDeps = SignalDeps & { readonly reserved: () => ReadonlyMap<NodeId, ReadonlySet<number>> };

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

// src/sim/world.ts:134-140 variable NO_RUNTIME
/** What `SimWorld.rt` answers for a lane nobody has been on: empty, not a ghost, frozen against writes. */
const NO_RUNTIME: LaneletRuntime = Object.freeze({
  id: '' as LaneletId,
  order: Object.freeze([]) as unknown as VehicleId[],
  shadows: Object.freeze([]) as unknown as VehicleId[],
  ghost: false,
  ghostSince: 0,
});

// src/sim/world.ts:153-1302 class SimWorld
export class SimWorld { … }

// src/sim/world.ts:155-155 field SimWorld.graph
readonly graph = new LaneletGraph();

// src/sim/world.ts:171-171 field SimWorld.rebindLanes
/** The lanes and movements the builds since the last rebind took or made again (`vehiclesReached`); `rebindAll` after a whole build. */
  private readonly rebindLanes = new Set<LaneletId>();

// src/sim/world.ts:186-186 field SimWorld.runtime
/** @mapWide One per lanelet a vehicle (or its shadow) has been on; written only through `runtimeFor`. */
  readonly runtime = new Map<LaneletId, LaneletRuntime>();

// src/sim/world.ts:191-191 field SimWorld.occupied
/** Lanes with a vehicle on them: written only by `enterLanelet`/`exitLanelet`, the only writers of `order`. */
  readonly occupied = new Set<LaneletId>();

// src/sim/world.ts:197-197 field SimWorld.ghostCandidates
/** Runtime lanes no longer in the graph (an edit took them): stage 7's work list (`collectGhosts`). */
  readonly ghostCandidates = new Set<LaneletId>();

// src/sim/world.ts:215-215 field SimWorld.mergeTurn
/** The lane that last sent a vehicle into each lane two or more lanes merge into, for the zipper */
  readonly mergeTurn = new Map<LaneletId, LaneletId>();

// src/sim/world.ts:233-233 field SimWorld.roadLength
/** The summed length of the roads, kept by what each rebuild changed (`world/networkLength.ts`). */
  readonly roadLength = new NetworkLength();

// src/sim/world.ts:242-242 field SimWorld.entryDemand
/** Traffic waiting to come in at each boundary entry lane */
  /** Per entry: the next arrival, the rate it was drawn at, and the arrivals waiting outside. */
  readonly entryDemand = new Map<LaneletId, { next: number; rate: number; waiting: number[] }>();

// src/sim/world.ts:262-265 field SimWorld.reservationsChecked
/** Each vehicle's reservation chain as last found valid (`pipeline.ts` reconcileReservedChain): walked again only when what it reads moved. */
  readonly reservationsChecked = new WeakMap<Vehicle, {
    readonly lanelet: LaneletId; readonly admitted: LaneletId | null; readonly reserved: readonly LaneletId[]; readonly reservedLength: number;
    readonly route: readonly LaneletId[]; readonly routeLength: number; readonly topology: object;
  }>();

// src/sim/world.ts:288-288 method SimWorld.topologyToken
/** The topology as a token for the worker's own frame bookkeeping: the value is the revision, never a product key. */
  topologyToken(): number;

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

// src/sim/world.ts:434-442 method SimWorld.junctionHolders
/** The vehicles holding or clearing a movement, or on a connector, now - in no set order */
  junctionHolders(): Vehicle[];

// src/sim/world.ts:445-458 method SimWorld.vehiclesNear
/** The vehicles on the lanes within `reach` of a point and a body's length and a lane's width more, in id order: DO NOT walk the fleet to find who is near. */
  vehiclesNear(x: number, y: number, reach: number): Vehicle[];

// src/sim/world.ts:461-475 method SimWorld.someVehicleNear
/** Whether `test` holds for one of `vehiclesNear`'s vehicles, asked in NO set order: for a yes or no that does not depend on it (a walker's gap, `walk.ts`). */
  someVehicleNear(x: number, y: number, reach: number, test: (v: Vehicle) => boolean): boolean;

// src/sim/world.ts:478-478 field SimWorld.nearLanes
/** `vehiclesNear`'s and `someVehicleNear`'s lanes, made once. */
  private readonly nearLanes: Lanelet[] = [];

// src/sim/world.ts:498-507 method SimWorld.inNodeOrder
/** @rebuild The junctions and signals in node order, made again after a change when a reader of every one asks. */
  private inNodeOrder(): { readonly nodes: readonly NodeId[]; readonly signalised: readonly SignalController[] };

// src/sim/world.ts:519-523 method SimWorld.junctionOfLane
/** The junction a lane is an approach of (in its `inbound`), from the graph: no map of every approach of the map to keep. */
  private junctionOfLane(id: LaneletId): NodeId | undefined;

// src/sim/world.ts:546-581 method SimWorld.signalsToStep
/** DO NOT walk every signal of the map a tick to pass over those asleep: their time keeps itself (`SignalClock`), only the awake are stepped */
  signalsToStep(deps: WorldSignalDeps): readonly SignalController[];

// src/sim/world.ts:641-641 field SimWorld.prepGraph
/** The first half of `rebuildTopology`: lanelets, conflict zones, claims and the signal controllers */
  /** A second lanelet graph, built from the same document, that is never run */
  private prepGraph: LaneletGraph | null = null;

// src/sim/world.ts:644-672 method SimWorld.prepareVehicleTopology
/** @rebuild The slow half of `rebuildVehicleTopology` ahead of time, in steps */
  *prepareVehicleTopology(measurer?: PairMeasurer): Generator<void | typeof WAITING, void>;

// src/sim/world.ts:676-680 method SimWorld.prepReach
private prepReach(changed: GraphChange | null): ReadonlySet<NodeId> | null;

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

// src/sim/world.ts:919-944 method SimWorld.stepControlAdvisor
/** Stage 0's measuring of the junctions: DO NOT evaluate every junction of the map, only those whose control the flows can still change */
  stepControlAdvisor(dt: number): void;

// src/sim/world.ts:947-961 method SimWorld.rebuildWalkTopology
/** The second half: the footway graph and its crossings, and the controllers again */
  rebuildWalkTopology(): void;

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

// src/sim/world.ts:1230-1234 method SimWorld.laneHead
/** The vehicle nearest the exit of a lanelet, or undefined. */
  laneHead(id: LaneletId): Vehicle | undefined;

// src/sim/world.ts:1243-1248 method SimWorld.addShadow
/** Registers a vehicle's body as still occupying the lane it is leaving. */
  addShadow(v: Vehicle, lanelet: LaneletId, offset: number, clearAt: number): void;

// src/sim/world.ts:1261-1284 method SimWorld.bodiesIn
/** Every body in a lane with its front arc position there */
  bodiesIn(id: LaneletId): { vehicle: Vehicle; s: number }[];

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
