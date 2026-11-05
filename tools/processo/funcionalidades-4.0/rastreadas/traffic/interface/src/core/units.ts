// What the feature uses of src/core/units.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/units.ts:2-2 variable METERS_PER_UNIT
/** The engine has exactly ONE unit system: the world unit. */
export const METERS_PER_UNIT: number = 1;

// src/core/units.ts:3-3 variable UNITS_PER_METER
export const UNITS_PER_METER = 1 / METERS_PER_UNIT;

// src/core/units.ts:6-6 function m
/** Converts a real-world metre figure into world units. Build-time only. */
export const m = (metres: number): number => …;

// src/core/units.ts:12-12 variable TUNED_UNIT
/** The world unit the game was drawn in until it moved to metres: 0.4 m. */
export const TUNED_UNIT = m(0.4);

// src/core/units.ts:15-16 function stepsIn
/** How many steps of `stepMetres` a length in world units holds, read to a millionth of a step, so a count rounded */
export const stepsIn = (units: number, stepMetres: number): number => …;

// src/core/units.ts:19-19 function kmh
/** Converts a km/h figure into world units per second. Build-time only. */
export const kmh = (kmPerHour: number): number => …;
