// What the feature uses of src/sim/ambient/ambient.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/ambient/ambient.ts:62-982 class AmbientWorld
export class AmbientWorld { … }

// src/sim/ambient/ambient.ts:120-153 method AmbientWorld.reset
/** Everything made is taken away (a new map). */
  reset(w: SimWorld): void;

// src/sim/ambient/ambient.ts:155-195 method AmbientWorld.step
step(w: SimWorld): void;

// src/sim/ambient/ambient.ts:198-222 method AmbientWorld.trimToCeilings
/** Down to the population the machine sustains (`SimWorld.populationShare`, `sim/populationGovernor.ts`) */
  private trimToCeilings(w: SimWorld): void;

// src/sim/ambient/ambient.ts:250-289 method AmbientWorld.people
private people(w: SimWorld, focus: NonNullable<SimWorld['focus']>, reach: number, want: number): void;

// src/sim/ambient/ambient.ts:422-457 method AmbientWorld.walkFrom
/** A walk from `from` (a road end, a lot's door with `fromLot`, a point on a footway) */
  private walkFrom(w: SimWorld, from: { node: number; x: number; y: number }, fromLot: DoorWay | null, toLot: DoorWay | null): number | null;

// src/sim/ambient/ambient.ts:471-490 method AmbientWorld.keepWalking
/** The walkers whose walk does not end at a road end or at a lot's door, sent on before they get to its end */
  private keepWalking(w: SimWorld): void;

// src/sim/ambient/ambient.ts:588-595 method AmbientWorld.pickPlace
/** A lot with a way to its door, at random by the people it has on its street now coming out (`out`) or going in */
  private pickPlace(w: SimWorld, hour: number, out: boolean, not: DoorWay | null): DoorWay | null;

// src/sim/ambient/ambient.ts:598-614 method AmbientWorld.company
/** Now and then somebody walking with a walker just made (`walkTrip` with) */
  private company(w: SimWorld, leader: number, age: PersonAgeClass, from: { x: number; y: number }, to: { x: number; y: number }, roams: boolean,
    head: readonly { x: number; y: number }[] = NO_WAY, tail: readonly { x: number; y: number }[] = NO_WAY): void;

// src/sim/ambient/ambient.ts:985-985 variable WALK_DECAY
/** Distance decay of a walk from one lot to another, u: the gravity model's negative exponential, e^(-d/L) */
const WALK_DECAY = m(470);

// src/sim/ambient/ambient.ts:988-988 variable NO_WAY
const NO_WAY: readonly { x: number; y: number }[] = [];

// src/sim/ambient/ambient.ts:991-995 function reversed
/** A lot's way the other way round: from the gate's foot on the footway in to the door. */
function reversed(way: DoorWay): readonly { x: number; y: number }[];
