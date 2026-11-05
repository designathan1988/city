// What the feature uses of src/sim/vehicles/obstacles.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/obstacles.ts:20-56 function pedestrianAhead
/** Stop short of a zebra span somebody is on; null when the path is clear. */
function pedestrianAhead(w: SimWorld, v: Vehicle): Obstacle | null;

// src/sim/vehicles/obstacles.ts:60-72 function crossingIdsAt
function crossingIdsAt(w: SimWorld, node: NodeId): readonly CrossingId[];

// src/sim/vehicles/obstacles.ts:106-106 variable PED_STOP_MARGIN
/** Extra space left in front of the zebra band (`CrossingSpan.along` already clears it). */
const PED_STOP_MARGIN = 0.5;

// src/sim/vehicles/obstacles.ts:109-212 function longitudinalConstraints
/** Turns the world into constraints for one vehicle. */
export function longitudinalConstraints(w: SimWorld, v: Vehicle): ConstraintSet;
