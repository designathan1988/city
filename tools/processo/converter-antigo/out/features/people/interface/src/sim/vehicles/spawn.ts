// What the feature uses of src/sim/vehicles/spawn.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/spawn.ts:32-45 function trafficTarget
/** Density ceiling on the fleet, from total road length. */
export function trafficTarget(w: SimWorld): number;

// src/sim/vehicles/spawn.ts:98-158 function stepDispatch
/** Brings traffic in at the edges of the map. */
export function stepDispatch(w: SimWorld, enabled: boolean): void;

// src/sim/vehicles/spawn.ts:167-226 function spawnAt
/** Puts one vehicle on the map at the start of an entry lane, if the lane has room behind whatever is already on it. */
function spawnAt(w: SimWorld, id: string): boolean;

// src/sim/vehicles/spawn.ts:280-292 function birthSpeed
/** How fast a vehicle arrives from off the map */
function birthSpeed(driver: ReturnType<typeof makeDriver>, v0: number, usual: number, gap: number, leaderSpeed: number): number;

// src/sim/vehicles/spawn.ts:295-325 function stepDespawn
/** Removes the vehicles that reached the end of a road and stopped there: the only place a car leaves the map */
export function stepDespawn(w: SimWorld): void;
