// What the feature uses of src/sim/params.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/params.ts:25-30 function laneChangeLength
/** Road length over which a lane change of offset `start` is driven at `speed`. */
export function laneChangeLength(start: number, speed: number, bodyLength: number): number;

// src/sim/params.ts:47-47 variable JAM_GAP
/** Bumper-to-bumper spacing at a standstill. */
export const JAM_GAP = m(2);

// src/sim/params.ts:49-49 variable MAX_VEHICLE_LENGTH
/** Longest vehicle in the fleet, used to size link capacity. */
const MAX_VEHICLE_LENGTH = m(9.8);

// src/sim/params.ts:51-51 variable L_MIN
/** Shortest link that can hold the largest vehicle plus its jam gap. */
export const L_MIN = MAX_VEHICLE_LENGTH + JAM_GAP + m(2);

// src/sim/params.ts:54-54 variable REQUEST_TIME
/** A vehicle requests entry from this far back — generous by design. */
export const REQUEST_TIME = 3.0;

// src/sim/params.ts:55-55 variable REQUEST_MIN_DISTANCE
export const REQUEST_MIN_DISTANCE = m(8);

// src/sim/params.ts:58-58 variable CONVOY_ROLLING
/** Speed above which a vehicle counts as rolling rather than queueing. */
export const CONVOY_ROLLING = m(0.2);

// src/sim/params.ts:61-66 variable CRITICAL_GAP
/** Critical gaps for yielding movements, in seconds. */
export const CRITICAL_GAP = {
  through: 4.5,
  right: 4.0,
  left: 5.5,
  uturn: 6.5,
} as const;

// src/sim/params.ts:71-71 variable CRITICAL_GAP_FLOOR
export const CRITICAL_GAP_FLOOR = 3.0;

// src/sim/params.ts:77-90 variable SIGNAL
/** Signal timing, in seconds. */
export const SIGNAL = {
  minGreen: 6,
  // The widest built-in boulevard crossing is about 55 world units.
  baseGreen: 20,
  maxGreen: 34,
  /** Target green of an exclusive stage serving turns that are otherwise permissive. */
  exclusiveGreen: 10,
  amber: 3.2,
  minAllRed: 1.2,
  /** A group red for longer than this many cycles is promoted. */
  starvationCycles: 1.5,
  /** Uncontrolled nodes hold a permanent green. */
  uncontrolledGreen: 3600,
} as const;

// src/sim/params.ts:131-131 variable DRIVER_NOISE
/** Free-flow speed sampling. */
export const DRIVER_NOISE = { lo: 0.92, hi: 1.08 } as const;
