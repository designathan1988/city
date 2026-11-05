// What the feature uses of src/core/scalar.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/scalar.ts:6-6 variable EPS
/** Generic floating-point slop for "are these the same point/number". */
export const EPS = 1e-9;

// src/core/scalar.ts:9-9 variable GEO_EPS
/** Geometric slop — below this (0.04 mm), two positions are one position. */
export const GEO_EPS = m(0.00004);

// src/core/scalar.ts:12-12 variable COARSE_EPS
/** Comparison slop for a quantity that has already been through arithmetic. */
export const COARSE_EPS = 1e-6;

// src/core/scalar.ts:45-45 variable MIN_RIBBON
/** Shortest ribbon (drawn road body) we will ever emit: 20 cm. */
export const MIN_RIBBON = m(0.2);

// src/core/scalar.ts:58-59 function clamp
export const clamp = (v: number, lo: number, hi: number): number => …;
