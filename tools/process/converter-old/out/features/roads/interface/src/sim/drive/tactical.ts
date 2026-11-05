// What the feature uses of src/sim/drive/tactical.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/drive/tactical.ts:11-11 variable CHANGE_COST
/** Drive v2, tactical layer: which LANE a trip is driven in. */

/** Seconds a lane change costs the trip, per lane crossed. Also the hysteresis. */
const CHANGE_COST = 4;

// src/sim/drive/tactical.ts:13-13 variable ROOM_PER_LANE
/** Road a change needs, per lane crossed. */
const ROOM_PER_LANE = m(24);

// src/sim/drive/tactical.ts:15-23 interface TripPlan
export interface TripPlan {
  /** Drivable from the start lane: links joined by their connectors */
  readonly route: LaneletId[];
  /** The sibling lane to move into before leaving the start lane, or null. */
  readonly changeTo: LaneletId | null;
  /** With `changeTo`: the connector the trip leaves that lane by. */
  readonly intent: LaneletId | null;
  readonly cost: number;
}

// src/sim/drive/tactical.ts:26-36 function connectorCost
/** Seconds to drive on through a connector and the link it leads to, with its congestion. */
function connectorCost(w: SimWorld, connectorId: LaneletId, body: BodyClass, shape: string): number;

// src/sim/drive/tactical.ts:39-140 function planTrip
/** DO NOT go back to Dijkstra's order (cost alone) */
export function planTrip(
  w: SimWorld,
  start: LaneletId,
  startS: number,
  goal: LaneletId,
  body: BodyClass,
  startChange = true,
  /** The vehicle's shape (`VehicleShape`): who may drive a bus lane. */
  shape = 'car',
  /** Whether a plan that changes lanes first needs its route as driven without the change (a second search): DO NOT ask it when the route is let go (`router.ts` extend, a bus's check). */
  fallback = true,
): TripPlan | null;

// src/sim/drive/tactical.ts:143-147 function relax
/** DO NOT make anything for a state before knowing it is kept: most are not (the allocation profile, 2026-10-17: `planTrip` 317 MB in the simulation's worker). */
function relax(scratch: SearchScratch, state: number, cost: number, toGo: (slot: number) => number, from: number, via: number): void;
