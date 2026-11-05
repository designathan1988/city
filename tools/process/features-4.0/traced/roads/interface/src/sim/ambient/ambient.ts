// What the feature uses of src/sim/ambient/ambient.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/ambient/ambient.ts:26-26 variable END_LOOK
/** The life of the scenery, as GTA makes it: nobody lives here. */

/** How far round a road end its footway points are looked for, u. */
const END_LOOK = m(16);

// src/sim/ambient/ambient.ts:36-36 variable MAKE_PER_LOOK
/** Most made in one look: the population fills over a second or two. */
const MAKE_PER_LOOK = 8;

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

// src/sim/ambient/ambient.ts:310-351 method AmbientWorld.roadEnds
/** DO NOT look round every node of the map when the network moves: every road drawn probed the town's road ends again. */
  private roadEnds(w: SimWorld): { node: number; x: number; y: number }[];

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
