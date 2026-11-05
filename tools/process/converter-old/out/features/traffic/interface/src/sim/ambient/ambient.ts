// What the feature uses of src/sim/ambient/ambient.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/ambient/ambient.ts:28-28 variable MAX_WALKERS
/** Most people on foot at once, and cars driving, and the cars driving when zoomed out. */
const MAX_WALKERS = 260;

// src/sim/ambient/ambient.ts:29-29 variable MAX_DRIVERS
const MAX_DRIVERS = 180;

// src/sim/ambient/ambient.ts:30-30 variable MAX_DRIVERS_FAR
const MAX_DRIVERS_FAR = 110;

// src/sim/ambient/ambient.ts:34-34 variable TRIM_PER_LOOK
/** Most walkers, and most drivers, taken away a look down to the machine's ceilings (`trimToCeilings`). */
const TRIM_PER_LOOK = 10;

// src/sim/ambient/ambient.ts:36-36 variable MAKE_PER_LOOK
/** Most made in one look: the population fills over a second or two. */
const MAKE_PER_LOOK = 8;

// src/sim/ambient/ambient.ts:38-38 variable OPEN_CARS
/** A map just opened is filled this many cars and people a tick, for at most this many ticks */
const OPEN_CARS = 2;

// src/sim/ambient/ambient.ts:43-43 variable PARKED
/** Share of the bays with a car in them. */
const PARKED = 0.62;

// src/sim/ambient/ambient.ts:46-48 variable TRAFFIC
/** The kinds of car that stand in bays and join the traffic (buses run their lines, `sim/transit`). */
export const TRAFFIC: readonly (readonly [Archetype, number])[] = ARCHETYPES
  .filter((a) => a.id !== 'bus')
  .map((a) => [a, a.id === 'truck' ? a.weight * 0.4 : a.weight] as const);

// src/sim/ambient/ambient.ts:49-49 variable PARKED_KINDS
const PARKED_KINDS = ARCHETYPES.filter((a) => a.id === 'hatch' || a.id === 'sedan' || a.id === 'suv' || a.id === 'van');

// src/sim/ambient/ambient.ts:52-60 interface SceneryCar
/** DO NOT stand a vehicle in every bay of the region: the scenery's car in a bay is this row of its place and look, a vehicle only when the lots drive it out (`release`) */
export interface SceneryCar extends StandingCar {
  free: FreePose;
  /** Its bay's place (`spotKey`). */
  readonly spot: number;
  /** The seed its look was drawn from (`stand`): its driver is drawn from it when it is driven out. */
  readonly seed: number;
  /** The lane its bay opened on when it was stood: the vehicle's first. */
  readonly lanelet: LaneletId;
}

// src/sim/ambient/ambient.ts:62-982 class AmbientWorld
export class AmbientWorld { … }

// src/sim/ambient/ambient.ts:64-64 field AmbientWorld.enabled
/** On in the game (`main.ts`); off under test. */
  enabled = false;

// src/sim/ambient/ambient.ts:68-68 field AmbientWorld.parked
/** The cars standing in bays: drawn, solid to walkers, never stepped; the scenery's as rows (`SceneryCar`). */
  readonly parked: StandingCar[] = [];

// src/sim/ambient/ambient.ts:70-70 field AmbientWorld.standing
/** The members of `parked` put there by the scenery or through `addParked` (`isParked`). */
  private readonly standing = new Set<StandingCar>();

// src/sim/ambient/ambient.ts:72-72 field AmbientWorld.parkedAt
/** Each car's place in `parked`, for its removal in one step (`removeParked`). */
  private readonly parkedAt = new Map<StandingCar, number>();

// src/sim/ambient/ambient.ts:74-74 field AmbientWorld.movers
/** The cars of `parked` that may move while they stand in it: the vehicles put there through `addParked` (the scenery's rows never move) */
  readonly movers = new Set<Vehicle>();

// src/sim/ambient/ambient.ts:76-76 field AmbientWorld.parkedLog
/** DO NOT make a reader compare the whole list every tick to find what changed: every parked car of the map, each tick. */
  private parkedLog: { readonly car: StandingCar; readonly added: boolean }[] = [];

// src/sim/ambient/ambient.ts:84-84 field AmbientWorld.sceneryAt
/** The scenery's own cars standing in bays, by the bay's place (`spotKey`). */
  private readonly sceneryAt = new Map<number, SceneryCar>();

// src/sim/ambient/ambient.ts:86-86 field AmbientWorld.sceneryWatch
/** Told the bay's place each time a scenery's car comes into a bay or leaves it (the lots' free bays, `lotTraffic.ts`); null: nobody. */
  sceneryWatch: ((spot: number) => void) | null = null;

// src/sim/ambient/ambient.ts:96-96 field AmbientWorld.sighted
/** The drivers made here that may be on screen now: never taken off the road at a dead end (`vehicles/spawn.ts` stepDespawn). */
  readonly sighted = new Set<number>();

// src/sim/ambient/ambient.ts:120-153 method AmbientWorld.reset
/** Everything made is taken away (a new map). */
  reset(w: SimWorld): void;

// src/sim/ambient/ambient.ts:155-195 method AmbientWorld.step
step(w: SimWorld): void;

// src/sim/ambient/ambient.ts:198-222 method AmbientWorld.trimToCeilings
/** Down to the population the machine sustains (`SimWorld.populationShare`, `sim/populationGovernor.ts`) */
  private trimToCeilings(w: SimWorld): void;

// src/sim/ambient/ambient.ts:240-240 field AmbientWorld.walls
/** DO NOT make it again on `buildings.revision` */
  private readonly walls = new SightWalls();

// src/sim/ambient/ambient.ts:243-248 method AmbientWorld.behindWall
/** True when a wall stands between the eye and (x, y), and between the eye and each side of it `pad` across. */
  private behindWall(w: SimWorld, ex: number, ey: number, x: number, y: number, pad: number): boolean;

// src/sim/ambient/ambient.ts:250-289 method AmbientWorld.people
private people(w: SimWorld, focus: NonNullable<SimWorld['focus']>, reach: number, want: number): void;

// src/sim/ambient/ambient.ts:354-364 method AmbientWorld.peopleTarget
/** How many people on foot the panel asks for: its count, or, following the city */
  private peopleTarget(w: SimWorld): number;

// src/sim/ambient/ambient.ts:367-419 method AmbientWorld.edgePeople
/** People on foot, up to the number chosen, from where people come from: a road end (from off the map) or a lot's gate */
  private edgePeople(w: SimWorld): void;

// src/sim/ambient/ambient.ts:422-457 method AmbientWorld.walkFrom
/** A walk from `from` (a road end, a lot's door with `fromLot`, a point on a footway) */
  private walkFrom(w: SimWorld, from: { node: number; x: number; y: number }, fromLot: DoorWay | null, toLot: DoorWay | null): number | null;

// src/sim/ambient/ambient.ts:471-490 method AmbientWorld.keepWalking
/** The walkers whose walk does not end at a road end or at a lot's door, sent on before they get to its end */
  private keepWalking(w: SimWorld): void;

// src/sim/ambient/ambient.ts:504-512 method AmbientWorld.open
/** The panel's numbers put on the streets of a map just opened, a slice a tick (`OPEN_CARS`, `OPEN_PEOPLE`) */
  private open(w: SimWorld): void;

// src/sim/ambient/ambient.ts:520-533 method AmbientWorld.openCars
/** Cars on link lanes of the whole map, each lane drawn by its length */
  private openCars(w: SimWorld, n: number): void;

// src/sim/ambient/ambient.ts:577-579 method AmbientWorld.refreshPlaces
/** The buildings people walk into, taken in from the diary */
  private refreshPlaces(w: SimWorld): void;

// src/sim/ambient/ambient.ts:582-585 method AmbientWorld.placesReady
/** Whether a lot may be walked to: one whose way is known to be walkable, or not yet worked out. */
  private placesReady(w: SimWorld): boolean;

// src/sim/ambient/ambient.ts:588-595 method AmbientWorld.pickPlace
/** A lot with a way to its door, at random by the people it has on its street now coming out (`out`) or going in */
  private pickPlace(w: SimWorld, hour: number, out: boolean, not: DoorWay | null): DoorWay | null;

// src/sim/ambient/ambient.ts:598-614 method AmbientWorld.company
/** Now and then somebody walking with a walker just made (`walkTrip` with) */
  private company(w: SimWorld, leader: number, age: PersonAgeClass, from: { x: number; y: number }, to: { x: number; y: number }, roams: boolean,
    head: readonly { x: number; y: number }[] = NO_WAY, tail: readonly { x: number; y: number }[] = NO_WAY): void;

// src/sim/ambient/ambient.ts:651-695 method AmbientWorld.traffic
private traffic(w: SimWorld, focus: NonNullable<SimWorld['focus']>, reach: number, want: number): void;

// src/sim/ambient/ambient.ts:698-706 method AmbientWorld.lanesAround
/** The link lanes whose bounds come within `reach` of the focus, each with its middle: where cars can join near the view. */
  private lanesAround(w: SimWorld, focus: { x: number; y: number }, reach: number): { id: string; length: number; x: number; y: number }[];

// src/sim/ambient/ambient.ts:709-712 method AmbientWorld.openingLanes
/** The link lanes of the map, each with its middle, for the opening's cars drawn by length (`openCars`). */
  private openingLanes(w: SimWorld): { id: string; length: number; x: number; y: number }[];

// src/sim/ambient/ambient.ts:715-726 method AmbientWorld.listLanes
/** @rebuild Listed only while a map is opening (`OPEN_TICKS`), when the lanes changed, with their running sum. */
  private listLanes(w: SimWorld): void;

// src/sim/ambient/ambient.ts:731-731 field AmbientWorld.vacated
/** Bays whose scenery car the lots drove out (`spotKey`): left empty when the bays are made again. */
  private readonly vacated = new Set<number>();

// src/sim/ambient/ambient.ts:752-754 method AmbientWorld.isParked
/** DO NOT ask `parked.includes` per car per tick: the list holds every parked car of the map. */
  isParked(body: StandingCar): boolean;

// src/sim/ambient/ambient.ts:767-776 method AmbientWorld.removeParked
/** DO NOT `indexOf` and `splice` it out: every parked car of the map, for each car of the lots that drives out */
  removeParked(body: StandingCar): void;

// src/sim/ambient/ambient.ts:784-788 method AmbientWorld.parkedSince
/** The cars that came into `parked` (`added`) and went since `serial`, oldest first; null: read it whole. `reader` has then read up to `parkedSerial`: what all readers read is let go. */
  parkedSince(serial: number, reader?: object): readonly { readonly car: StandingCar; readonly added: boolean }[] | null;

// src/sim/ambient/ambient.ts:790-794 method AmbientWorld.logParked
private logParked(car: StandingCar, added: boolean): void;

// src/sim/ambient/ambient.ts:830-849 method AmbientWorld.parking
/** DO NOT stand every bay of the town again at each change: every bay of the map */
  private parking(w: SimWorld): void;

// src/sim/ambient/ambient.ts:852-873 method AmbientWorld.parkInWindow
/** The parked cars of the micro window: sources come into it stood, those gone out of it taken away, those changed in it stood again. */
  private parkInWindow(w: SimWorld, win: { readonly x: number; readonly y: number; readonly r: number } | null, changed: readonly string[] | null,
    movers: ReadonlyMap<number, readonly Vehicle[]>): void;

// src/sim/ambient/ambient.ts:876-885 method AmbientWorld.moverCells
/** The cars of the lots (`movers`) filed by `MOVER_CELL` cells of where they stand. */
  private moverCells(): Map<number, Vehicle[]>;

// src/sim/ambient/ambient.ts:888-903 method AmbientWorld.standSource
/** The scenery's cars of one source of bays stood again: those there now off, a car in each of its bays now (`stand`) */
  private standSource(w: SimWorld, source: string, fill: boolean, movers: ReadonlyMap<number, readonly Vehicle[]>): void;

// src/sim/ambient/ambient.ts:906-943 method AmbientWorld.stand
/** A scenery car in a bay, or none (the bay left empty): the same row in the same bay each time, its look drawn from where the bay is. */
  private stand(w: SimWorld, bay: Bay, was: SceneryCar | undefined, movers: ReadonlyMap<number, readonly Vehicle[]>): SceneryCar | null;

// src/sim/ambient/ambient.ts:946-953 method AmbientWorld.takeScenery
/** A scenery car taken out of its bay (the bays changed): off the cars standing, and out of its source. */
  private takeScenery(car: SceneryCar): void;

// src/sim/ambient/ambient.ts:956-964 method AmbientWorld.leaveSource
/** A scenery car no longer counted with its source of bays. */
  private leaveSource(car: SceneryCar): void;

// src/sim/ambient/ambient.ts:969-969 field AmbientWorld.buildingMix
/** DO NOT count every building again under a key with `buildings.revision` */
  private readonly buildingMix = new BuildingMix();

// src/sim/ambient/ambient.ts:971-974 method AmbientWorld.mixAround
private mixAround(w: SimWorld, x: number, y: number, r: number): Mix;

// src/sim/ambient/ambient.ts:977-977 field AmbientWorld.street
/** DO NOT put a one-slot cache back in front of it: three questions a look */
  private readonly street = new StreetLength();

// src/sim/ambient/ambient.ts:979-981 method AmbientWorld.roadAround
private roadAround(w: SimWorld, x: number, y: number, r: number): number;

// src/sim/ambient/ambient.ts:985-985 variable WALK_DECAY
/** Distance decay of a walk from one lot to another, u: the gravity model's negative exponential, e^(-d/L) */
const WALK_DECAY = m(470);

// src/sim/ambient/ambient.ts:987-987 function newPlaces
/** The buildings people walk into, by the lot's way (`lotDoors.ts`). */
const newPlaces = (): LotPlaces<DoorWay> => …;

// src/sim/ambient/ambient.ts:988-988 variable NO_WAY
const NO_WAY: readonly { x: number; y: number }[] = [];

// src/sim/ambient/ambient.ts:989-989 variable REVERSED
const REVERSED = new WeakMap<DoorWay, readonly { x: number; y: number }[]>();

// src/sim/ambient/ambient.ts:991-995 function reversed
/** A lot's way the other way round: from the gate's foot on the footway in to the door. */
function reversed(way: DoorWay): readonly { x: number; y: number }[];

// src/sim/ambient/ambient.ts:998-998 variable MOVER_CELL
/** How near a car of the lots holds a bay (`AmbientWorld.stand`), and the cells they are filed by. */
const MOVER_CELL = m(1.5);

// src/sim/ambient/ambient.ts:999-999 function moverCell
const moverCell = (i: number, j: number): number => …;
