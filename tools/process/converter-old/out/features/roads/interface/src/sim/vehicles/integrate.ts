// What the feature uses of src/sim/vehicles/integrate.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/integrate.ts:14-14 variable CLEARANCE_EPSILON
const CLEARANCE_EPSILON = COARSE_EPS;

// src/sim/vehicles/integrate.ts:18-135 function integrateAll
/** The ONLY writer of `s`, `v` and `lanelet`. */
export function integrateAll(w: SimWorld): void;

// src/sim/vehicles/integrate.ts:138-189 function applyLaneChange
/** Performs a lane change the lane-change stage already cleared as safe. */
function applyLaneChange(w: SimWorld, v: Vehicle): ReturnType<SimWorld['lanelet']>;

// src/sim/vehicles/integrate.ts:198-209 function discardUntravelledFromNewClearances
/** Removes overshoot that the next stop line rejected from newly-created tokens. */
function discardUntravelledFromNewClearances(
  v: Vehicle,
  clearancesAtStart: ReadonlySet<Vehicle['clearingConnectors'][number]>,
  distance: number,
): void;

// src/sim/vehicles/integrate.ts:212-238 function advanceClearanceTokens
/** Advances rear-clearance ownership without consuming the next admission. */
function advanceClearanceTokens(
  w: SimWorld,
  v: Vehicle,
  distance: number,
  clearancesAtStart: ReadonlySet<Vehicle['clearingConnectors'][number]>,
): void;

// src/sim/vehicles/integrate.ts:241-277 function advance
/** Moves a vehicle onto the next lanelet on its route. */
function advance(w: SimWorld, v: Vehicle, carried: number): boolean;

// src/sim/vehicles/integrate.ts:279-326 function advanceTo
function advanceTo(w: SimWorld, v: Vehicle, target: string, carried: number): boolean;
