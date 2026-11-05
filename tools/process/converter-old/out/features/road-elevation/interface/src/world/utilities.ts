// What the feature uses of src/world/utilities.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/utilities.ts:31-31 variable DEFAULT_POLE_SPACING
/** Default distance between poles along a drawn run, in world units. */
export const DEFAULT_POLE_SPACING = ROAD_TUNING.poles.spacing;

// src/world/utilities.ts:32-32 variable MIN_POLE_SPACING
export const MIN_POLE_SPACING = ROAD_TUNING.poles.minSpacing;

// src/world/utilities.ts:33-33 variable MAX_POLE_SPACING
export const MAX_POLE_SPACING = ROAD_TUNING.poles.maxSpacing;

// src/world/utilities.ts:43-49 interface UtilityPole
export interface UtilityPole {
  readonly id: PoleId;
  x: number;
  y: number;
  /** Whether this pole carries a street light. */
  lamp: boolean;
}

// src/world/utilities.ts:51-55 interface UtilitySpan
export interface UtilitySpan {
  readonly id: SpanId;
  a: PoleId;
  b: PoleId;
}

// src/world/utilities.ts:72-72 variable POLE_LAMP_MODES
/** What the poles of a new run carry: a street light on none, every other one, or all of them. */
export const POLE_LAMP_MODES = ['none', 'alternate', 'all'] as const;

// src/world/utilities.ts:73-73 type PoleLampMode
export type PoleLampMode = (typeof POLE_LAMP_MODES)[number];
