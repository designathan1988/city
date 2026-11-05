// What the feature uses of src/world/utilities.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/utilities.ts:72-72 variable POLE_LAMP_MODES
/** What the poles of a new run carry: a street light on none, every other one, or all of them. */
export const POLE_LAMP_MODES = ['none', 'alternate', 'all'] as const;

// src/world/utilities.ts:73-73 type PoleLampMode
export type PoleLampMode = (typeof POLE_LAMP_MODES)[number];
