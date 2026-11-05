// What the feature uses of src/sim/vehicles/leaderIndex.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/leaderIndex.ts:14-81 function findLeader
/** Nearest obstacle ahead, following the vehicle's own planned lanelet sequence; `out` written as the winner (a set's slot reused, `idm.ts`). */
export function findLeader(w: SimWorld, v: Vehicle, out?: MutableObstacle | null): Obstacle | null;

// src/sim/vehicles/leaderIndex.ts:99-115 function shadowsAhead
/** Rears of bodies sliding out of a lane, ahead of `s` on it. */
function shadowsAhead(
  w: SimWorld,
  laneId: LaneletId,
  s: number,
  self: number,
): { vehicle: Vehicle; rear: number }[];

// src/sim/vehicles/leaderIndex.ts:118-165 function divergeObstacle
/** The body of a vehicle that took a DIFFERENT movement out of the same lane */
export function divergeObstacle(w: SimWorld, v: Vehicle): Obstacle | null;

// src/sim/vehicles/leaderIndex.ts:194-201 function upcomingLanelets
/** The next `n` lanelets on the vehicle's route, excluding the current one. */
function upcomingLanelets(w: SimWorld, v: Vehicle, n: number): LaneletId[];
