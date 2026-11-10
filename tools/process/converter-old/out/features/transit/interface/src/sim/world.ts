// What the feature uses of src/sim/world.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/world.ts:37-37 variable FOCUS_WINDOW
/** The most of the window round the focus, where the scenery's cars are made (`AmbientWorld.traffic`). */
const FOCUS_WINDOW = m(600);

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

// src/sim/world.ts:153-1302 class SimWorld
export class SimWorld { … }

// src/sim/world.ts:154-154 field SimWorld.clock
readonly clock = new SimClock();

// src/sim/world.ts:163-167 field SimWorld.vehicles
/** @mapWide Every vehicle on the map; each filed by its route as it comes and goes (`routes`). */
  readonly vehicles = new VersionedMap<VehicleId, Vehicle>({
    set: (v, was) => { if (was && was !== v) this.routes.unfile(was); this.routes.file(v); },
    delete: (v) => this.routes.unfile(v),
    clear: () => this.routes.clear(),
  });

// src/sim/world.ts:174-174 field SimWorld.microWindow
/** Whether the conflicts are kept only in the micro window (the game, `simHost.ts`); the specs keep every junction's. */
  private microWindow = false;

// src/sim/world.ts:226-226 getter SimWorld.rng
/** The world's random streams, forked from its seed (`reset` forks them again). */
  get rng(): SimStreams;

// src/sim/world.ts:229-229 field SimWorld.nextVehicleId
nextVehicleId = 1;

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

// src/sim/world.ts:361-363 method SimWorld.lanelet
lanelet(id: LaneletId): Lanelet | undefined;

// src/sim/world.ts:1184-1202 method SimWorld.enterLanelet
/** Inserts a vehicle into a lanelet's occupancy list, keeping it sorted. */
  enterLanelet(v: Vehicle, id: LaneletId, recordVolume = true): void;

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
