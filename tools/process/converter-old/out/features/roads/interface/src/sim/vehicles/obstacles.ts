// What the feature uses of src/sim/vehicles/obstacles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/obstacles.ts:17-17 variable CLOSURE_STOP
/** How far short of a closed stretch of lane the traffic stops. */
export const CLOSURE_STOP = m(1.5);

// src/sim/vehicles/obstacles.ts:20-56 function pedestrianAhead
/** Stop short of a zebra span somebody is on; null when the path is clear. */
function pedestrianAhead(w: SimWorld, v: Vehicle): Obstacle | null;

// src/sim/vehicles/obstacles.ts:59-59 variable CROSSING_IDS
/** DO NOT drop every node's for an edit: the whole map's were spelt again after each road drawn. */
const CROSSING_IDS = new WeakMap<SimWorld, { seen: object | null; byNode: Map<NodeId, CrossingId[]> }>();

// src/sim/vehicles/obstacles.ts:60-72 function crossingIdsAt
function crossingIdsAt(w: SimWorld, node: NodeId): readonly CrossingId[];

// src/sim/vehicles/obstacles.ts:75-103 function mergeObstacle
/** The vehicle ahead on ANOTHER movement into the lane this one runs into. */
function mergeObstacle(w: SimWorld, v: Vehicle): Obstacle | null;

// src/sim/vehicles/obstacles.ts:106-106 variable PED_STOP_MARGIN
/** Extra space left in front of the zebra band (`CrossingSpan.along` already clears it). */
const PED_STOP_MARGIN = 0.5;

// src/sim/vehicles/obstacles.ts:109-212 function longitudinalConstraints
/** Turns the world into constraints for one vehicle. */
export function longitudinalConstraints(w: SimWorld, v: Vehicle): ConstraintSet;
