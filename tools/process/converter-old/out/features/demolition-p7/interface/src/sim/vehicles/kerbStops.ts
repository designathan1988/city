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
