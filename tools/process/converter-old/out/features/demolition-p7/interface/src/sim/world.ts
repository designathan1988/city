// What the feature uses of src/sim/world.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/world.ts:39-39 variable BODY_OFF_LANE
/** How far a vehicle's pose can lie outside its lane's bounds (`vehiclesNear`): the longest body (its rear axle still on the lane behind) and a lane change's sideways reach. */
const BODY_OFF_LANE = m(20);

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

// src/sim/world.ts:260-260 field SimWorld.city
/** The city's services - the time of day, the public transport (`sim/city/city.ts`) - stepped by the pipeline. */
  readonly city = new City();

// src/sim/world.ts:267-267 field SimWorld.ambient
/** The life of the scenery: people and traffic made round the view (`sim/ambient`). */
  readonly ambient = new AmbientWorld();

// src/sim/world.ts:293-301 constructor SimWorld.constructor
constructor(
    readonly doc: RoadDoc,
    readonly net: Network,
    private readonly seed = 0x5eed,
    /** The buildings as traffic reads them: the document's own (a spec), or the worker's table of traffic records (`simHost.ts`). */
    readonly buildings: TrafficBuildings = docBuildings(doc),
  );

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

// src/sim/world.ts:685-687 method SimWorld.walkwaysNow
/** The walkways of the network as it is now. */
  walkwaysNow(): WalkGraph;

// src/sim/world.ts:1230-1234 method SimWorld.laneHead
/** The vehicle nearest the exit of a lanelet, or undefined. */
  laneHead(id: LaneletId): Vehicle | undefined;

// src/sim/world.ts:1261-1284 method SimWorld.bodiesIn
/** Every body in a lane with its front arc position there */
  bodiesIn(id: LaneletId): { vehicle: Vehicle; s: number }[];

// src/sim/world.ts:1338-1400 class VersionedMap
/** A Map that counts its changes of membership, for caches over it, and keeps its values in the order of their keys */
class VersionedMap<K extends number, V> extends Map<K, V> { … }
