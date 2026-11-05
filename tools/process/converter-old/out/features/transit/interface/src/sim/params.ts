// What the feature uses of src/sim/params.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/params.ts:54-54 variable REQUEST_TIME
/** A vehicle requests entry from this far back — generous by design. */
export const REQUEST_TIME = 3.0;

// src/sim/params.ts:55-55 variable REQUEST_MIN_DISTANCE
export const REQUEST_MIN_DISTANCE = m(8);

// src/sim/params.ts:131-131 variable DRIVER_NOISE
/** Free-flow speed sampling. */
export const DRIVER_NOISE = { lo: 0.92, hi: 1.08 } as const;
