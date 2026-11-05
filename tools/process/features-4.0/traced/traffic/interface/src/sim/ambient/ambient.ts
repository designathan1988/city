// What the feature uses of src/sim/ambient/ambient.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/ambient/ambient.ts:46-48 variable TRAFFIC
/** The kinds of car that stand in bays and join the traffic (buses run their lines, `sim/transit`). */
export const TRAFFIC: readonly (readonly [Archetype, number])[] = ARCHETYPES
  .filter((a) => a.id !== 'bus')
  .map((a) => [a, a.id === 'truck' ? a.weight * 0.4 : a.weight] as const);

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

// src/sim/ambient/ambient.ts:84-84 field AmbientWorld.sceneryAt
/** The scenery's own cars standing in bays, by the bay's place (`spotKey`). */
  private readonly sceneryAt = new Map<number, SceneryCar>();

// src/sim/ambient/ambient.ts:86-86 field AmbientWorld.sceneryWatch
/** Told the bay's place each time a scenery's car comes into a bay or leaves it (the lots' free bays, `lotTraffic.ts`); null: nobody. */
  sceneryWatch: ((spot: number) => void) | null = null;

// src/sim/ambient/ambient.ts:96-96 field AmbientWorld.sighted
/** The drivers made here that may be on screen now: never taken off the road at a dead end (`vehicles/spawn.ts` stepDespawn). */
  readonly sighted = new Set<number>();

// src/sim/ambient/ambient.ts:731-731 field AmbientWorld.vacated
/** Bays whose scenery car the lots drove out (`spotKey`): left empty when the bays are made again. */
  private readonly vacated = new Set<number>();

// src/sim/ambient/ambient.ts:752-754 method AmbientWorld.isParked
/** DO NOT ask `parked.includes` per car per tick: the list holds every parked car of the map. */
  isParked(body: StandingCar): boolean;

// src/sim/ambient/ambient.ts:767-776 method AmbientWorld.removeParked
/** DO NOT `indexOf` and `splice` it out: every parked car of the map, for each car of the lots that drives out */
  removeParked(body: StandingCar): void;

// src/sim/ambient/ambient.ts:790-794 method AmbientWorld.logParked
private logParked(car: StandingCar, added: boolean): void;

// src/sim/ambient/ambient.ts:956-964 method AmbientWorld.leaveSource
/** A scenery car no longer counted with its source of bays. */
  private leaveSource(car: SceneryCar): void;
