// What the feature uses of src/sim/world.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/world.ts:39-39 variable BODY_OFF_LANE
/** How far a vehicle's pose can lie outside its lane's bounds (`vehiclesNear`): the longest body (its rear axle still on the lane behind) and a lane change's sideways reach. */
const BODY_OFF_LANE = m(20);

// src/sim/world.ts:46-46 type WorldSignalDeps
/** What the signals read of the world (`SignalDeps`), and the reservations by node, which wake a sleeping one (`SimWorld.signalsToStep`). */
export type WorldSignalDeps = SignalDeps & { readonly reserved: () => ReadonlyMap<NodeId, ReadonlySet<number>> };

// src/sim/world.ts:61-65 variable NO_PEDESTRIANS
/** Nobody walks until an engine is installed (`SimWorld.usePedestrianEngine`). */
const NO_PEDESTRIANS: PedestrianEngine = {
  kind: 'people',
  beginTick() {}, dispatch() {}, step() {}, rebind() {}, publish() {}, audit() {}, reset() {},
  bridge: { hailable: () => null, board: () => null, alight: () => false, anyoneWithin: () => false },
};

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

// src/sim/world.ts:157-157 field SimWorld.sidewalks
readonly sidewalks = new SidewalkGraph();

// src/sim/world.ts:159-159 field SimWorld.signalPosts
/** The traffic signal posts by place, kept from the junctions' controls (`signalChangesSince`): the walkers keep clear of them. */
  readonly signalPosts = new SignalPostIndex();

// src/sim/world.ts:163-167 field SimWorld.vehicles
/** @mapWide Every vehicle on the map; each filed by its route as it comes and goes (`routes`). */
  readonly vehicles = new VersionedMap<VehicleId, Vehicle>({
    set: (v, was) => { if (was && was !== v) this.routes.unfile(was); this.routes.file(v); },
    delete: (v) => this.routes.unfile(v),
    clear: () => this.routes.clear(),
  });

// src/sim/world.ts:182-182 field SimWorld.pedEngine
/** What moves the people (`people/engine.ts`); everything else reaches them through it. */
  pedEngine: PedestrianEngine = NO_PEDESTRIANS;

// src/sim/world.ts:199-199 field SimWorld.asleep
/** Signals asleep (`signalsToStep`), by node: rested in green with nobody near on their last step. */
  private readonly asleep = new Map<NodeId, SignalController>();

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

// src/sim/world.ts:226-226 getter SimWorld.rng
/** The world's random streams, forked from its seed (`reset` forks them again). */
  get rng(): SimStreams;

// src/sim/world.ts:229-229 field SimWorld.nextVehicleId
nextVehicleId = 1;

// src/sim/world.ts:237-237 field SimWorld.laneReaches
/** The road ends each lane reaches, per body class, kept by what each edit changed (`routing/laneReach.ts`). */
  readonly laneReaches = new LaneReaches();

// src/sim/world.ts:249-249 field SimWorld.pedestrianIntensity
pedestrianIntensity = 1;

// src/sim/world.ts:251-251 field SimWorld.trafficCount
/** How many cars and how many people on foot the player wants on the map (the panel's Traffic and People, `main.ts`). */
  trafficCount: number | null = null;

// src/sim/world.ts:254-254 field SimWorld.followCity
/** The game's way (`main.ts`): the panel's Traffic and People are shares of the city's own density */
  followCity = false;

// src/sim/world.ts:258-258 field SimWorld.edgeTraffic
/** Traffic from outside: cars at the map's edge entries, people at doors and road ends (set by `City.step`). */
  edgeTraffic = true;

// src/sim/world.ts:260-260 field SimWorld.city
/** The city's services - the time of day, the public transport (`sim/city/city.ts`) - stepped by the pipeline. */
  readonly city = new City();

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

// src/sim/world.ts:293-301 constructor SimWorld.constructor
constructor(
    readonly doc: RoadDoc,
    readonly net: Network,
    private readonly seed = 0x5eed,
    /** The buildings as traffic reads them: the document's own (a spec), or the worker's table of traffic records (`simHost.ts`). */
    readonly buildings: TrafficBuildings = docBuildings(doc),
  );

// src/sim/world.ts:304-310 method SimWorld.usePedestrianEngine
/** Switches the model that moves the people. */
  usePedestrianEngine(engine: PedestrianEngine): void;

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

// src/sim/world.ts:395-397 method SimWorld.controller
controller(node: NodeId): SignalController | undefined;

// src/sim/world.ts:434-442 method SimWorld.junctionHolders
/** The vehicles holding or clearing a movement, or on a connector, now - in no set order */
  junctionHolders(): Vehicle[];

// src/sim/world.ts:478-478 field SimWorld.nearLanes
/** `vehiclesNear`'s and `someVehicleNear`'s lanes, made once. */
  private readonly nearLanes: Lanelet[] = [];

// src/sim/world.ts:601-606 method SimWorld.wakeSignal
private wakeSignal(node: NodeId): void;

// src/sim/world.ts:644-672 method SimWorld.prepareVehicleTopology
/** @rebuild The slow half of `rebuildVehicleTopology` ahead of time, in steps */
  *prepareVehicleTopology(measurer?: PairMeasurer): Generator<void | typeof WAITING, void>;

// src/sim/world.ts:683-683 field SimWorld.walkways
/** The residents' walkways (`world/walkways.ts`), kept by what each edit changed. */
  private readonly walkways = new Walkways();

// src/sim/world.ts:685-687 method SimWorld.walkwaysNow
/** The walkways of the network as it is now. */
  walkwaysNow(): WalkGraph;

// src/sim/world.ts:700-700 field SimWorld.topologyLog
/** What each build of the lane graph changed */
  readonly topologyLog = new ChangeLog<GraphChange>();

// src/sim/world.ts:947-961 method SimWorld.rebuildWalkTopology
/** The second half: the footway graph and its crossings, and the controllers again */
  rebuildWalkTopology(): void;

// src/sim/world.ts:964-973 method SimWorld.crossingsAt
/** Crossing ids at a node, one per incident segment. */
  crossingsAt(node: NodeId): CrossingId[];

// src/sim/world.ts:983-1054 method SimWorld.makeSignalDeps
private makeSignalDeps(): { readonly deps: WorldSignalDeps; readonly fresh: () => void };

// src/sim/world.ts:1090-1101 method SimWorld.junctionQuiet
/** Nobody near a signal's junction (`SignalDeps.quiet`): its approaches empty, nobody at its crossings, nothing reserved there. */
  private junctionQuiet(c: SignalController, reserved: () => ReadonlyMap<NodeId, ReadonlySet<number>>): boolean;

// src/sim/world.ts:1184-1202 method SimWorld.enterLanelet
/** Inserts a vehicle into a lanelet's occupancy list, keeping it sorted. */
  enterLanelet(v: Vehicle, id: LaneletId, recordVolume = true): void;

// src/sim/world.ts:1230-1234 method SimWorld.laneHead
/** The vehicle nearest the exit of a lanelet, or undefined. */
  laneHead(id: LaneletId): Vehicle | undefined;

// src/sim/world.ts:1261-1284 method SimWorld.bodiesIn
/** Every body in a lane with its front arc position there */
  bodiesIn(id: LaneletId): { vehicle: Vehicle; s: number }[];

// src/sim/world.ts:1286-1292 method SimWorld.removeVehicle
removeVehicle(v: Vehicle): void;

// src/sim/world.ts:1338-1400 class VersionedMap
/** A Map that counts its changes of membership, for caches over it, and keeps its values in the order of their keys */
class VersionedMap<K extends number, V> extends Map<K, V> { … }

// src/sim/world.ts:1349-1356 method VersionedMap.set
override set(key: K, value: V): this;
