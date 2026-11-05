// What the feature uses of src/sim/vehicles/kerbStops.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/kerbStops.ts:13-13 type ErrandKind
/** Somebody getting out of a car at the kerb, or getting in. */

export type ErrandKind = 'drop' | 'pick' | 'service' | 'deliver';

// src/sim/vehicles/kerbStops.ts:15-15 type TaskKind
/** What one task of a stop does: somebody gets out, or somebody gets in. */
type TaskKind = 'drop' | 'pick';

// src/sim/vehicles/kerbStops.ts:17-17 type KerbStopPhase
type KerbStopPhase = 'approach' | 'halt' | 'hold' | 'fetch' | 'open' | 'transfer' | 'close';

// src/sim/vehicles/kerbStops.ts:20-20 type KerbPerson
/** The person moving between the seat and the footway. */
type KerbPerson = Boarder;

// src/sim/vehicles/kerbStops.ts:22-48 interface KerbStop
export interface KerbStop {
  readonly kind: TaskKind;
  readonly lanelet: LaneletId;
  /** Arc position of the FRONT of the vehicle where it stops: planned, then where it stood still (`halt`). */
  at: number;
  readonly door: number;
  readonly seat: number;
  phase: KerbStopPhase;
  /** Seconds in the phase. */
  t: number;
  /** Seconds the whole stop has taken, for the give-up guard. */
  elapsed: number;
  /** For a pick-up, the pedestrian to collect until they are taken in. */
  pedId: number | null;
  person: KerbPerson | null;
  /** Seconds the transfer takes, fixed when it starts. */
  transferTime: number;
  /** A pick-up: seconds the person needs to walk to the door, and has walked. */
  fetchTime: number;
  walked: number;
  /** A drop-off that does not become a pedestrian: the truck's mate, who comes back. */
  keep: boolean;
  /** Seconds the person stands on the footway before walking back (a delivery). */
  hold: number;
  /** Whether the person moves between a seat and the door (cars, cabs) or just steps in or out (buses). */
  seatStage: boolean;
}

// src/sim/vehicles/kerbStops.ts:72-72 variable KERB_PACE
/** A person's pace to or from the door, world units a second. */
const KERB_PACE = m(1.1);

// src/sim/vehicles/kerbStops.ts:74-74 variable DOOR_CLEAR
/** Radius round the door's swing that must be free of people before it opens. */
const DOOR_CLEAR = m(1.4);

// src/sim/vehicles/kerbStops.ts:78-78 variable PICK_REACH
/** How far along the footway a pick-up looks for somebody walking alone. */
const PICK_REACH = m(40);

// src/sim/vehicles/kerbStops.ts:83-83 variable BUS_FETCH
/** How far somebody will walk to catch a bus that has stopped. */
const BUS_FETCH = m(45);

// src/sim/vehicles/kerbStops.ts:86-86 variable PERSON_BASE
/** First id of the people who ride in vehicles, clear of any pedestrian id. */
const PERSON_BASE = 1 << 24;

// src/sim/vehicles/kerbStops.ts:88-88 variable SEAT_STRIDE
/** DO NOT go back to a bare 8: a body of nine seats or more gave its last seats the ids of the next vehicle's first ones */
const SEAT_STRIDE = Math.max(8, ...ARCHETYPES.map((a) => a.seats));

// src/sim/vehicles/kerbStops.ts:92-92 type Seated
/** Who sits in a seat. */
/** What `seatPerson` reads of a vehicle (the simulation's own, or the page's view of it, `host/simClient.ts`). */
type Seated = { readonly id: number; readonly people: readonly number[]; readonly peopleAge: readonly (PersonAgeClass | undefined)[]; readonly archetype: { readonly doorsPerSide: number } };

// src/sim/vehicles/kerbStops.ts:95-95 interface SeatPerson
/** The person a seat holds, as `seatPerson` reads it. */
export interface SeatPerson { seed: number; gender: PersonGender; ageClass: PersonAgeClass }

// src/sim/vehicles/kerbStops.ts:102-114 function seatPersonInto
/** `seatPerson` written into `out`: DO NOT make an object a seat a frame (the page draws every seated person each frame). */
export function seatPersonInto(v: Seated, seat: number, out: SeatPerson): SeatPerson;

// src/sim/vehicles/kerbStops.ts:117-123 function seatAgeClass
/** `seatPerson(v, seat).ageClass` without the object (AGT-32: read for every seat of every car drawn, each frame). */
export function seatAgeClass(v: Seated, seat: number): PersonAgeClass;

// src/sim/vehicles/kerbStops.ts:126-159 function assignOccupancy
/** Fixed at spawn: which seats are taken, and whether the trip has an errand. */
export function assignOccupancy(w: SimWorld, v: Vehicle): void;

// src/sim/vehicles/kerbStops.ts:182-189 function doorAlongFromFront
/** Where a door is along the body, from the front of the vehicle, as a share of its length: front row and rear row. */
function doorAlongFromFront(v: Vehicle, door: number): number;

// src/sim/vehicles/kerbStops.ts:192-275 function stepKerbStops
/** One step of every errand. The only writer of `kerbStop` and `doors`. */
export function stepKerbStops(w: SimWorld): void;

// src/sim/vehicles/kerbStops.ts:282-288 function abandon
function abandon(v: Vehicle): void;

// src/sim/vehicles/kerbStops.ts:303-362 function plan
/** Chooses where to stop on the current link, or waits for a later one. */
function plan(w: SimWorld, v: Vehicle): void;

// src/sim/vehicles/kerbStops.ts:365-397 function planBusStop
/** A bus stop: one or two passengers off by the middle door, and whoever is walking alone nearby on by the front door */
function planBusStop(w: SimWorld, v: Vehicle, lanelet: LaneletId, earliest: number, latest: number): void;

// src/sim/vehicles/kerbStops.ts:399-402 function newStop
function newStop(kind: TaskKind, lanelet: LaneletId, at: number, seat: number, door = seat): KerbStop;

// src/sim/vehicles/kerbStops.ts:405-421 function queuedAhead
/** Somebody standing still or queued in front of the stop would make it a queue, not a stop. */
function queuedAhead(w: SimWorld, v: Vehicle, at: number): boolean;

// src/sim/vehicles/kerbStops.ts:434-453 function footwayBeside
/** The nearest point of a walking footway on the kerb side at `s`, if one runs there. */
function footwayBeside(w: SimWorld, lanelet: LaneletId, s: number): { x: number; y: number; t: { x: number; y: number } } | null;

// src/sim/vehicles/kerbStops.ts:468-487 function roomToOpen
/** The door's swing and the footway spot are free of people. */
function roomToOpen(w: SimWorld, v: Vehicle, stop: KerbStop): boolean;

// src/sim/vehicles/kerbStops.ts:515-527 function hail
/** The car has stopped for somebody: they see it and walk over. */
function hail(w: SimWorld, v: Vehicle, stop: KerbStop): void;

// src/sim/vehicles/kerbStops.ts:529-557 function completeTransfer
function completeTransfer(w: SimWorld, v: Vehicle, stop: KerbStop): void;

// src/sim/vehicles/kerbStops.ts:560-565 function setSeat
/** DO NOT `delete` an entry or write past the end */
function setSeat<T>(list: T[], seat: number, value: T | undefined): void;
