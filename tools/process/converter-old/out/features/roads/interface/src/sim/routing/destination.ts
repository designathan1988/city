// What the feature uses of src/sim/routing/destination.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/routing/destination.ts:9-12 function reachableExits
/** DO NOT search the graph from each entry */
function reachableExits(w: SimWorld, start: LaneletId, body: BodyClass): readonly LaneletId[];

// src/sim/routing/destination.ts:15-19 function chooseVehicleDestination
/** Chooses one reachable trip endpoint, once, when a vehicle enters the city. */
export function chooseVehicleDestination(w: SimWorld, start: LaneletId, body: BodyClass): LaneletId | null;

// src/sim/routing/destination.ts:22-76 function routeToDestination
/** Least-time lanelet route to a fixed endpoint, with congestion priced at planning time. */
export function routeToDestination(w: SimWorld, start: LaneletId, goal: LaneletId, body: BodyClass): LaneletId[] | null;
