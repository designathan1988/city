// What the feature uses of src/sim/vehicles/spawn.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/vehicles/spawn.ts:55-57 function roadLength
/** The road length of a network, kept by what each rebuild changed (`world/networkLength.ts`). */
function roadLength(w: SimWorld): number;

// src/sim/vehicles/spawn.ts:60-83 function entryLanes
/** DO NOT walk the map's lanes for an edit. */
function entryLanes(w: SimWorld): readonly string[];

// src/sim/vehicles/spawn.ts:84-84 variable entries
const entries = new WeakMap<SimWorld, { seen: object | null; readonly lanes: Set<string>; sorted: readonly string[] | null }>();

// src/sim/vehicles/spawn.ts:87-90 function isEntry
/** A link long enough to arrive on, starting at a node on the edge of the map. */
function isEntry(w: SimWorld, id: string): boolean;

// src/sim/vehicles/spawn.ts:167-226 function spawnAt
/** Puts one vehicle on the map at the start of an entry lane, if the lane has room behind whatever is already on it. */
function spawnAt(w: SimWorld, id: string): boolean;

// src/sim/vehicles/spawn.ts:229-229 type DestinationPicker
/** Where a new car's trip ends, from its first lane: a map exit by default (`chooseVehicleDestination`), the micro window's edge for the meso's cars (`Meso.handoffDestination`). */
export type DestinationPicker = (w: SimWorld, start: LaneletId, body: BodyClass) => LaneletId | null;

// src/sim/vehicles/spawn.ts:232-277 function spawnVehicleAt
/** Puts one vehicle of `arch` on lane `id` with its front at arc `s` */
export function spawnVehicleAt(w: SimWorld, id: string, s: number, arch: Archetype, pick: DestinationPicker = chooseVehicleDestination): number | null;

// src/sim/vehicles/spawn.ts:280-292 function birthSpeed
/** How fast a vehicle arrives from off the map */
function birthSpeed(driver: ReturnType<typeof makeDriver>, v0: number, usual: number, gap: number, leaderSpeed: number): number;

// src/sim/vehicles/spawn.ts:295-325 function stepDespawn
/** Removes the vehicles that reached the end of a road and stopped there: the only place a car leaves the map */
export function stepDespawn(w: SimWorld): void;
