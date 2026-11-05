// What the feature uses of src/sim/params.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/params.ts:13-13 variable LANE_CHANGE_HEADING_SLOW
/** A lane change is a PATH, not a slide. */
const LANE_CHANGE_HEADING_SLOW = 0.42;

// src/sim/params.ts:14-14 variable LANE_CHANGE_HEADING_FAST
const LANE_CHANGE_HEADING_FAST = 0.1;

// src/sim/params.ts:16-16 variable LANE_CHANGE_FAST_SPEED
/** Speed at which the shallow heading is reached, world units a second. */
const LANE_CHANGE_FAST_SPEED = kmh(45);

// src/sim/params.ts:18-18 variable LANE_CHANGE_MIN_TIME
/** Never quicker than this at any speed, seconds. */
const LANE_CHANGE_MIN_TIME = 2;

// src/sim/params.ts:20-20 variable LANE_CHANGE_MIN_BODIES
/** Shortest curve, in body lengths of the vehicle driving it. */
const LANE_CHANGE_MIN_BODIES = 2.5;

// src/sim/params.ts:22-22 variable PROFILE_PEAK
/** Peak slope of the quintic profile, as a multiple of its mean slope. */
const PROFILE_PEAK = 1.875;

// src/sim/params.ts:47-47 variable JAM_GAP
/** Bumper-to-bumper spacing at a standstill. */
export const JAM_GAP = m(2);

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

// src/sim/params.ts:69-69 variable IMPATIENCE_RATE
/** Waiting drivers accept smaller gaps, but never below the floor. */
export const IMPATIENCE_RATE = 0.06;

// src/sim/params.ts:70-70 variable IMPATIENCE_MAX
export const IMPATIENCE_MAX = 1.5;

// src/sim/params.ts:71-71 variable CRITICAL_GAP_FLOOR
export const CRITICAL_GAP_FLOOR = 3.0;

// src/sim/params.ts:74-74 variable WAIT_CEILING
/** Seconds a driver may wait for a gap before taking the next one offered. */
export const WAIT_CEILING = 12;

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

// src/sim/params.ts:93-112 variable PED
/** Pedestrian parameters. */
export const PED = {
  /** Design speed used to size clearance intervals. */
  designSpeed: m(1.0),
  meanSpeed: m(1.34),
  speedSd: m(0.2),
  minSpeed: m(0.8),
  maxSpeed: m(1.8),
  /** Reaction lag before stepping off the kerb. */
  startLag: 1.0,
  /** Following model. */
  jamGap: m(0.45),
  headway: 0.4,
  accel: m(1.2),
  /** Parallel walking files across a crossing. */
  files: 3,
  fileSpacing: m(0.7),
  criticalGap: 4.0,
  /** Probability a driver yields to a waiting pedestrian at an uncontrolled crossing. */
  courtesyYield: 0.35,
} as const;

// src/sim/params.ts:115-115 variable TRAFFIC_DENSITY
/** Vehicle population target, per unit of total road length. */
export const TRAFFIC_DENSITY = 1 / m(45);

// src/sim/params.ts:119-119 variable FLEET_CEILING
/** Safety ceilings on the agent population. */
// They are ALSO what the renderer sizes its instance buffers from
export const FLEET_CEILING = 1200;

// src/sim/params.ts:120-120 variable PED_CEILING
export const PED_CEILING = 1000;

// src/sim/params.ts:131-131 variable DRIVER_NOISE
/** Free-flow speed sampling. */
export const DRIVER_NOISE = { lo: 0.92, hi: 1.08 } as const;

// src/sim/params.ts:134-134 variable MAX_LATERAL_ACCEL
/** Speed cap through curvature, expressed as the lateral acceleration a driver accepts, which FALLS with speed. */
export const MAX_LATERAL_ACCEL = m(4.0);

// src/sim/params.ts:136-136 variable LATERAL_ACCEL_FALL
/** Reduction of the accepted lateral acceleration per unit of speed, per second. */
export const LATERAL_ACCEL_FALL = 0.09;

// src/sim/params.ts:137-137 variable MIN_LATERAL_ACCEL
export const MIN_LATERAL_ACCEL = m(1.8);
