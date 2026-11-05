// What the feature uses of src/sim/agents/lotDoors.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/lotDoors.ts:12-20 interface DoorWay
/** The way on foot between a lot's front door and the footway */

/** A lot's way on foot, world units. */
export interface DoorWay {
  readonly building: BuildingId;
  /** From the door, through the gate, to the gate's foot on the footway. */
  readonly out: readonly Vec2[];
  /** The lot's width along the street, u: how much street it fronts. */
  readonly frontage: number;
  /** The way is a street door's step (`streetWay`), not a gate's */
  readonly street?: true;
}
