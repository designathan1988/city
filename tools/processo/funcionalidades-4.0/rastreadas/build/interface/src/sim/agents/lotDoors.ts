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

// src/sim/agents/lotDoors.ts:28-28 variable DOOR_REACH
/** Farthest a door may be behind its gate. */
const DOOR_REACH = m(40);

// src/sim/agents/lotDoors.ts:30-30 variable DOOR_OFF
/** Where the door's walker stands: this far in front of the wall. */
const DOOR_OFF = m(0.45);

// src/sim/agents/lotDoors.ts:32-32 variable GATE_IN
/** Inside the gate, the point the way runs square through it from; outside, its foot on the footway. */
const GATE_IN = m(1.5);

// src/sim/agents/lotDoors.ts:33-33 variable GATE_OUT
const GATE_OUT = m(0.6);

// src/sim/agents/lotDoors.ts:48-95 function workOut
function workOut(b: TrafficBuilding): DoorWay | null;

// src/sim/agents/lotDoors.ts:98-109 function streetWay
/** A shop or a bar built up to the footway, with no people's gate: its front door opens onto the street */
function streetWay(b: TrafficBuilding): DoorWay | null;

// src/sim/agents/lotDoors.ts:112-126 function hitRings
/** Distance along the ray from (x, y) in (dx, dy) to the nearest side of the rings; null when it meets none. */
function hitRings(rings: readonly (readonly Vec2[])[], x: number, y: number, dx: number, dy: number): number | null;
