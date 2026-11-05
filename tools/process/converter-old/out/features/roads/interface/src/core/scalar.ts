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

// src/core/scalar.ts:15-15 variable FINE_EPS
/** Slop for a value derived by one or two operations rather than many. */
export const FINE_EPS = 1e-7;

// src/core/scalar.ts:18-18 variable DIV_EPS
/** Guard against dividing by a quantity that is numerically zero. */
export const DIV_EPS = 1e-12;

// src/core/scalar.ts:21-21 variable MIN_ITER_STEP
/** Floor on an iteration step, so a degenerate case cannot loop forever. */
export const MIN_ITER_STEP = 1e-3;

// src/core/scalar.ts:24-24 variable SIN_EPS
/** |sin(angle)| below this means two directions are parallel (~3 degrees). */
export const SIN_EPS = Math.sin((3 * Math.PI) / 180);

// src/core/scalar.ts:27-27 variable ACUTE_EPS
/** Wedge angles below this are "acute" and fall back to a bevel (~12 degrees). */
export const ACUTE_EPS = (12 * Math.PI) / 180;

// src/core/scalar.ts:30-30 variable MITER_LIMIT
/** Offset miter is clamped to this multiple of the offset distance. */
export const MITER_LIMIT = 4;

// src/core/scalar.ts:33-33 variable CORNER_MITER
/** Junction corner miter limit, as a multiple of the larger half-width. */
export const CORNER_MITER = 2.5;

// src/core/scalar.ts:39-39 variable ARC_TOL
/** Max chordal deviation when flattening an arc, in world units (8 mm). */
export const ARC_TOL = m(0.008);

// src/core/scalar.ts:42-42 variable TAPER_ANGLE
/** Taper slope used where two collinear legs have different half-widths. */
export const TAPER_ANGLE = (12 * Math.PI) / 180;

// src/core/scalar.ts:45-45 variable MIN_RIBBON
/** Shortest ribbon (drawn road body) we will ever emit: 20 cm. */
export const MIN_RIBBON = m(0.2);

// src/core/scalar.ts:48-48 variable SEAM_OVERLAP
/** How far a ribbon overlaps into its junction so nonzero fill hides the seam: 10 cm. */
export const SEAM_OVERLAP = m(0.1);

// src/core/scalar.ts:51-51 variable SURFACE_END_STEP
/** Extra end extension per nested surface level. */
export const SURFACE_END_STEP = m(0.00004);

// src/core/scalar.ts:58-59 function clamp
export const clamp = (v: number, lo: number, hi: number): number => …;

// src/core/scalar.ts:67-72 function normalizeAngle
/** Wraps an angle into (-PI, PI]. */
export function normalizeAngle(a: number): number;
