// What the feature uses of src/sim/crossings/permission.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/crossings/permission.ts:45-45 type Reservation
/** DO NOT key it by a `node:segment` text again */
type Reservation = { readonly v: Vehicle; readonly connector: string };

// src/sim/crossings/permission.ts:46-46 variable RESERVATIONS
const RESERVATIONS = new WeakMap<SimWorld, Map<NodeId, Reservation[]>>();

// src/sim/crossings/permission.ts:48-48 variable reservedIds
/** The connectors one vehicle reserves, this tick: cleared and refilled, never reallocated. */
const reservedIds: string[] = [];

// src/sim/crossings/permission.ts:106-106 variable PED_COURTESY
/** Seconds a car stands at a zebra giving way to people arriving at its kerb before it is its turn. */
export const PED_COURTESY = 8;

// src/sim/crossings/permission.ts:148-171 function vehicleBodyIntersectsCrossing
export function vehicleBodyIntersectsCrossing(w: SimWorld, vehicle: Vehicle, crossing: SidewalkEdge): boolean;
