// What the feature uses of src/sim/agents/lorryDocks.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lorryDocks.ts:90-94 interface LorryWay
/** A way a lorry drives, in the world: rear-axle poses with the direction each is reached in. */
export interface LorryWay {
  readonly poses: readonly Waypoint[];
  /** Where, along `poses`, the street part ends (arriving) or begins (leaving): the lanes it crosses are closed meanwhile. */
  readonly street: number;
}

// src/sim/agents/lorryDocks.ts:96-272 class Yard
export class Yard { … }
