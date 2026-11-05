// What the feature uses of src/core/units.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/units.ts:2-2 variable METERS_PER_UNIT
/** The engine has exactly ONE unit system: the world unit. */
export const METERS_PER_UNIT: number = 1;

// src/core/units.ts:3-3 variable UNITS_PER_METER
export const UNITS_PER_METER = 1 / METERS_PER_UNIT;

// src/core/units.ts:6-6 function m
/** Converts a real-world metre figure into world units. Build-time only. */
export const m = (metres: number): number => …;
