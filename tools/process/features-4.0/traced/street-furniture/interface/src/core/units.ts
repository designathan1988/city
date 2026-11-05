// What the feature uses of src/core/units.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/units.ts:6-6 function m
/** Converts a real-world metre figure into world units. Build-time only. */
export const m = (metres: number): number => …;

// src/core/units.ts:9-9 function perM
/** Converts a figure PER METRE (a frequency, a texture's repeats, pixels a metre, a density along a road) into the same per */
export const perM = (perMetre: number): number => …;

// src/core/units.ts:19-19 function kmh
/** Converts a km/h figure into world units per second. Build-time only. */
export const kmh = (kmPerHour: number): number => …;
