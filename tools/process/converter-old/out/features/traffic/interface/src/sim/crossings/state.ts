// What the feature uses of src/sim/crossings/state.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/crossings/state.ts:4-14 interface CrossingOccupant
/** What the rest of the simulation may know about the people at one crossing. */
export interface CrossingOccupant {
  readonly id: number;
  /** Distance from the `from` kerb. */
  readonly s: number;
  /** Walking from the `from` kerb towards the `to` kerb. */
  readonly forward: boolean;
  /** Walking speed, world units a second. */
  readonly v: number;
  /** Standing still on the crossing, waiting for something - very often for the very vehicle asking - rather than arriving. */
  readonly held: boolean;
}

// src/sim/crossings/state.ts:16-28 interface CrossingState
export interface CrossingState {
  /** Kerb to kerb, world units. */
  readonly length: number;
  /** Everybody on the crossing, in the order they stepped on. */
  readonly occupants: CrossingOccupant[];
  /** People waiting at each kerb to cross. */
  waitingFrom: number;
  waitingTo: number;
  /** Anybody at, or walking up to, a kerb meaning to cross here. */
  demand: boolean;
  /** Longest time anybody has waited at a kerb for this crossing, seconds. */
  longestWait: number;
}

// src/sim/crossings/state.ts:30-30 type CrossingStates
export type CrossingStates = Map<CrossingId, CrossingState>;
