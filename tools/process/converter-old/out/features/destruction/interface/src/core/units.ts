// What the feature uses of src/core/units.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/units.ts:6-6 function m
/** Converts a real-world metre figure into world units. Build-time only. */
export const m = (metres: number): number => …;

// src/core/units.ts:12-12 variable TUNED_UNIT
/** The world unit the game was drawn in until it moved to metres: 0.4 m. */
export const TUNED_UNIT = m(0.4);
