// What the feature uses of src/sim/params.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/params.ts:47-47 variable JAM_GAP
/** Bumper-to-bumper spacing at a standstill. */
export const JAM_GAP = m(2);

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

// src/sim/params.ts:119-119 variable FLEET_CEILING
/** Safety ceilings on the agent population. */
// They are ALSO what the renderer sizes its instance buffers from
export const FLEET_CEILING = 1200;

// src/sim/params.ts:120-120 variable PED_CEILING
export const PED_CEILING = 1000;

// src/sim/params.ts:131-131 variable DRIVER_NOISE
/** Free-flow speed sampling. */
export const DRIVER_NOISE = { lo: 0.92, hi: 1.08 } as const;
