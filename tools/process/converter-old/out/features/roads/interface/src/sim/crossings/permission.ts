// What the feature uses of src/sim/crossings/permission.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/crossings/permission.ts:17-42 function mayEnterCrossing
/** Whether a pedestrian may step off the kerb. */
export function mayEnterCrossing(w: SimWorld, crossing: SidewalkEdge, waited: number): boolean;

// src/sim/crossings/permission.ts:45-45 type Reservation
/** DO NOT key it by a `node:segment` text again */
type Reservation = { readonly v: Vehicle; readonly connector: string };

// src/sim/crossings/permission.ts:48-48 variable reservedIds
/** The connectors one vehicle reserves, this tick: cleared and refilled, never reallocated. */
const reservedIds: string[] = [];

// src/sim/crossings/permission.ts:49-52 function alreadyReserved
const alreadyReserved = (id: string): boolean =>;

// src/sim/crossings/permission.ts:54-75 function indexReservations
export function indexReservations(w: SimWorld): void;

// src/sim/crossings/permission.ts:77-87 function crossingReservedByVehicle
function crossingReservedByVehicle(w: SimWorld, node: NodeId, segment: number, crossing: CrossingId): boolean;

// src/sim/crossings/permission.ts:90-103 function turnOwedToVehicle
/** Whether a car at its line, at green */
function turnOwedToVehicle(w: SimWorld, node: NonNullable<SidewalkEdge["node"]>, crossing: string, controller: SignalController): boolean;

// src/sim/crossings/permission.ts:106-106 variable PED_COURTESY
/** Seconds a car stands at a zebra giving way to people arriving at its kerb before it is its turn. */
export const PED_COURTESY = 8;

// src/sim/crossings/permission.ts:109-111 function heldOnlyByPedestrians
/** Whether the only thing keeping a car at its line is people at or on a zebra */
function heldOnlyByPedestrians(w: SimWorld, v: Vehicle): boolean;

// src/sim/crossings/permission.ts:114-134 function pedGapAccepted
/** Gap acceptance against approaching traffic on the lanes being crossed. */
function pedGapAccepted(w: SimWorld, crossing: SidewalkEdge, waited: number): boolean;

// src/sim/crossings/permission.ts:137-146 function vehicleBodyOnCrossing
/** An approach vehicle's body can reach a zebra even without an admission token. */
function vehicleBodyOnCrossing(w: SimWorld, crossing: SidewalkEdge): boolean;

// src/sim/crossings/permission.ts:148-171 function vehicleBodyIntersectsCrossing
export function vehicleBodyIntersectsCrossing(w: SimWorld, vehicle: Vehicle, crossing: SidewalkEdge): boolean;
