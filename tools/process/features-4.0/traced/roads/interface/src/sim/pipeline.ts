// What the feature uses of src/sim/pipeline.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/pipeline.ts:21-26 interface StepOptions
export interface StepOptions {
  readonly traffic?: boolean;
  readonly pedestrians?: boolean;
  /** Milliseconds spent per stage, ADDED to on every step when given. */
  readonly timings?: Map<string, number>;
}

// src/sim/pipeline.ts:29-128 function step
/** @perFrame One fixed simulation step. */
export function step(w: SimWorld, opts: StepOptions = {}): void;

// src/sim/pipeline.ts:131-134 function rebindAgents
/** Re-binds agents to the rebuilt topology after a live edit. */
export function rebindAgents(w: SimWorld): void;

// src/sim/pipeline.ts:137-170 function rebindVehicles
/** @rebuild The vehicle half of `rebindAgents`: DO NOT walk the fleet, only those the edits reached (`vehiclesReached`); replanning all held an edit 4.5 s (1,200 cars, 2026-10-15). */
export function rebindVehicles(w: SimWorld): void;

// src/sim/pipeline.ts:178-178 variable tickWork
/** The work of the ticks: lanes stage 7 looked at, lanes retired, controllers stage 1 ran. */
export const tickWork = { cleanupLanes: 0, retired: 0, controllers: 0, rebound: 0 };

// src/sim/pipeline.ts:181-205 function collectGhosts
/** Retires lanelets that no longer exist once their occupants have left. */
function collectGhosts(w: SimWorld): void;

// src/sim/pipeline.ts:208-217 function updateStallCounters
/** Accumulates, per tick, how long each vehicle has stood still with a green signal and nothing in its way: DO NOT walk the fleet, only the lanes into a signal with somebody on them (off them both counters are nought, `SimWorld.enterLanelet`). */
function updateStallCounters(w: SimWorld): void;

// src/sim/pipeline.ts:220-248 function stallCounters
/** One vehicle's stall counters, on a lane into a signal. */
function stallCounters(w: SimWorld, v: Vehicle): void;

// src/sim/pipeline.ts:274-334 function ensureVehicleRoutes
/** Makes admission consume a real, connected route rather than a fallback. */
function ensureVehicleRoutes(w: SimWorld): void;

// src/sim/pipeline.ts:352-409 function reservedChainValid
function reservedChainValid(w: SimWorld, v: Vehicle, lane: NonNullable<ReturnType<SimWorld['lanelet']>>): boolean;

// src/sim/pipeline.ts:412-450 function pinReservedRoute
/** Reconstructs the route prefix pinned by the current grant and soft claim. */
function pinReservedRoute(w: SimWorld, v: Vehicle): boolean;
