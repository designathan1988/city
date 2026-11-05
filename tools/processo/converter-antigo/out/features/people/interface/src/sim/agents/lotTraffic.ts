// What the feature uses of src/sim/agents/lotTraffic.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lotTraffic.ts:45-45 variable ASK_WAY
/** Seconds a car off the road waits for people on its way before going on (they walk out of a car's ground, `walk.ts`). */
const ASK_WAY = 3;

// src/sim/agents/lotTraffic.ts:49-49 variable KINDS
/** The kinds of place a bay's building is (`ambient/demand.ts` kindOf), each a class of the draws; the last class, a building gone (weighs nought). */
const KINDS: readonly (keyof Mix)[] = ['home', 'shop', 'work', 'civic'];

// src/sim/agents/lotTraffic.ts:50-50 variable GONE
const GONE = KINDS.length;

// src/sim/agents/lotTraffic.ts:51-51 variable CLASSES
const CLASSES = KINDS.length + 1;

// src/sim/agents/lotTraffic.ts:53-53 type Phase
type Phase = 'coming' | 'parking' | 'parked' | 'leaving';

// src/sim/agents/lotTraffic.ts:55-81 interface LotCar
interface LotCar {
  readonly id: VehicleId;
  bay: Bay;
  phase: Phase;
  /** The car as drawn off the road; null while it drives in the traffic. */
  body: Vehicle | null;
  path: Manoeuvre | null;
  /** Seconds in the phase. */
  t: number;
  /** The lots' time (`LotTraffic.time`) its least stay parked is over at. */
  until: number;
  /** Sent out (`depart`): it leaves once its stay is over. */
  go: boolean;
  /** Seconds waited for people on the way. */
  waited: number;
  /** Holding the ground of the rest of its way: it drives on without stopping for anybody. */
  reserved: boolean;
  /** Its place in the lots' order (`track`), and its slot among the cars `depart` may draw (`waiting`). */
  readonly order: number;
  slot: number;
  /** Parked, its least stay over and not sent out (`ripen`): drawn by its bay's building's class. */
  ripe: boolean;
  kind: number;
  /** Where its bay is and where its body stands (`spotKey`): neither place a free bay. */
  readonly bayAt: number;
  bodyAt: number | null;
}

// src/sim/agents/lotTraffic.ts:84-89 interface GatedBay
/** A gated bay as the lots keep it: its place, its slot in the bays' order, its building's class. */
interface GatedBay {
  bay: Bay;
  readonly key: number;
  slot: number;
  kind: number;
}

// src/sim/agents/lotTraffic.ts:91-712 class LotTraffic
export class LotTraffic { … }

// src/sim/agents/lotTraffic.ts:92-92 field LotTraffic.cars
private readonly cars = new Map<VehicleId, LotCar>();

// src/sim/agents/lotTraffic.ts:93-93 field LotTraffic.nextOrder
private nextOrder = 0;

// src/sim/agents/lotTraffic.ts:94-94 field LotTraffic.clock
private clock = 0;

// src/sim/agents/lotTraffic.ts:96-96 field LotTraffic.baysRead
/** The bays' diary as last read (`bayChangesSince`), and the serial seen a look before it was read. */
  private baysRead = -1;

// src/sim/agents/lotTraffic.ts:97-97 field LotTraffic.baysSeen
private baysSeen = -1;

// src/sim/agents/lotTraffic.ts:108-108 field LotTraffic.carsAt
/** The cars of the lots by the place of their bay, how many of their bodies stand at each place, the places the lorries stand at during a look. */
  private readonly carsAt = new Map<number, LotCar[]>();

// src/sim/agents/lotTraffic.ts:109-109 field LotTraffic.bodies
private readonly bodies = new Map<number, number>();

// src/sim/agents/lotTraffic.ts:110-110 field LotTraffic.lorriesAt
private readonly lorriesAt = new Set<number>();

// src/sim/agents/lotTraffic.ts:112-112 field LotTraffic.waiting
/** The cars of the lots in their order, each of its class while it is ripe: those `depart` draws among. */
  private readonly waiting = new WeightedSlots(CLASSES);

// src/sim/agents/lotTraffic.ts:113-113 field LotTraffic.carBySlot
private readonly carBySlot: (LotCar | undefined)[] = [];

// src/sim/agents/lotTraffic.ts:115-115 field LotTraffic.stays
/** The parked cars' ids by when their least stay is over (`ripen`). */
  private readonly stays = new Heap();

// src/sim/agents/lotTraffic.ts:117-117 field LotTraffic.buildingsSeen
/** The buildings' diary as last read (`followBuildings`). */
  private buildingsSeen = -1;

// src/sim/agents/lotTraffic.ts:121-121 field LotTraffic.ambient
/** The scenery the bays ask who stands in them; it tells each change of a bay of its (`AmbientWorld.sceneryWatch`). */
  private ambient: AmbientWorld | null = null;

// src/sim/agents/lotTraffic.ts:122-122 field LotTraffic.sceneryMoved
private readonly sceneryMoved = (spot: number): void => this.refresh(spot);

// src/sim/agents/lotTraffic.ts:124-124 field LotTraffic.lorries
/** The lorries to the works' docks, stepped with the cars (`lorryTraffic.ts`). */
  readonly lorries = new LorryTraffic();

// src/sim/agents/lotTraffic.ts:126-126 field LotTraffic.called
/** Counters for probes: cars sent in, parked, driven out. */
  called = 0;

// src/sim/agents/lotTraffic.ts:127-127 field LotTraffic.parkedIn
parkedIn = 0;

// src/sim/agents/lotTraffic.ts:128-128 field LotTraffic.leftBy
leftBy = 0;

// src/sim/agents/lotTraffic.ts:131-131 field LotTraffic.active
/** DO NOT step every car of the lots a tick: those parked and not sent out stand still and are left out until `depart` sends them. */
  private readonly active = new Set<LotCar>();

// src/sim/agents/lotTraffic.ts:133-133 field LotTraffic.time
/** Seconds the lots have run: what a parked car's least stay is counted against (`LotCar.until`). */
  private time = 0;

// src/sim/agents/lotTraffic.ts:135-135 field LotTraffic.comingCount
/** The cars on their way, and those driving into or out of a bay, in all and by building: kept by each change of phase (`enter`, `track`, `untrack`). */
  private comingCount = 0;

// src/sim/agents/lotTraffic.ts:136-136 field LotTraffic.movingCount
private movingCount = 0;

// src/sim/agents/lotTraffic.ts:137-137 field LotTraffic.movingIn
private readonly movingIn = new Map<number, number>();

// src/sim/agents/lotTraffic.ts:145-158 method LotTraffic.track
/** A car taken into the lots' list, in its phase: its bay's place held, and its body's. */
  private track(id: VehicleId, bay: Bay, phase: Phase, body: Vehicle | null, go: boolean): void;

// src/sim/agents/lotTraffic.ts:220-252 method LotTraffic.step
/** Stage 2 (`sim/city/city.ts`): with the scenery on, cars go in and out of the lots. */
  step(w: SimWorld): void;

// src/sim/agents/lotTraffic.ts:260-260 field LotTraffic.owed
/** Trips owed to the lots (`TRIP_SECONDS`), fractions carried from look to look. */
  private owed = 0;

// src/sim/agents/lotTraffic.ts:268-307 method LotTraffic.depart
/** DO NOT build a pool of every ripe car and scenery's car a look: drawn by their running weights, the lots' cars in their order first, then the scenery's in the bays' order */
  private depart(w: SimWorld, target: number): void;

// src/sim/agents/lotTraffic.ts:326-367 method LotTraffic.readBays
/** DO NOT read the town's list (`collectBays`) on a change of `buildings.revision` */
  private readBays(w: SimWorld): void;

// src/sim/agents/lotTraffic.ts:370-387 method LotTraffic.fileBay
/** A gated bay filed at its place; a second bay at a place held takes it and keeps its slot, as the list of bays kept the place's turn. */
  private fileBay(w: SimWorld, bay: Bay): void;

// src/sim/agents/lotTraffic.ts:407-414 method LotTraffic.dropEveryBay
/** @rebuild Every gated bay let go: the bays read whole again (a map loaded, the diary unable to say which changed). */
  private dropEveryBay(): void;

// src/sim/agents/lotTraffic.ts:440-447 method LotTraffic.followBuildings
/** The bays and ripe cars of the buildings the diary says changed: their class read again (a draw weighs a building's kind). */
  private followBuildings(w: SimWorld): void;

// src/sim/agents/lotTraffic.ts:468-473 method LotTraffic.refreshBay
/** Free while no car of the lots holds it, no body of theirs or lorry stands on it and no scenery's car is in it; the scenery's while one is */
  private refreshBay(e: GatedBay): void;

// src/sim/agents/lotTraffic.ts:476-484 method LotTraffic.placeBody
/** A car's body counted where it stands now, the bay there no longer free. */
  private placeBody(car: LotCar): void;

// src/sim/agents/lotTraffic.ts:499-513 method LotTraffic.holdLorries
/** The places the lorries' bodies stand at held through a look's calls (their bays not free), or let go. */
  private holdLorries(hold: boolean): void;

// src/sim/agents/lotTraffic.ts:516-563 method LotTraffic.call
/** DO NOT draw among the bays near the view (`SimWorld.focus`); DO NOT list the free bays to draw one: their running weights by class */
  private call(w: SimWorld): boolean;

// src/sim/agents/lotTraffic.ts:567-585 method LotTraffic.coming
private coming(w: SimWorld, car: LotCar): void;

// src/sim/agents/lotTraffic.ts:672-675 method LotTraffic.drop
/** A car off the road taken out of the lots' list and of the drawn ones. */
  private drop(w: SimWorld, car: LotCar): void;

// src/sim/agents/lotTraffic.ts:678-700 method LotTraffic.mayGo
/** Whether nobody stands where the car will go over the rest of its way (its body as three discs every metre) */
  private mayGo(w: SimWorld, car: LotCar, body: Vehicle): boolean;

// src/sim/agents/lotTraffic.ts:715-718 function kindClass
/** A building's class in the draws: its kind of place (`KINDS`), or `GONE`. */
function kindClass(w: SimWorld, building: BuildingId): number;
