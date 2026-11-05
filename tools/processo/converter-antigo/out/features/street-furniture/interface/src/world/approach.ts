// What the feature uses of src/world/approach.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/approach.ts:7-7 variable CROSSWALK_DEPTH
/** Depth of the zebra along the direction of travel: 3 m, the Brazilian minimum for an urban crossing */
export const CROSSWALK_DEPTH = m(3);

// src/world/approach.ts:9-9 variable STOP_BAR_SETBACK
/** Clear space between the far edge of the zebra and the stop line. */
export const STOP_BAR_SETBACK = m(0.64);

// src/world/approach.ts:14-15 function crosswalkDistance
/** Distance from the node to the centre of the crossing. */
export const crosswalkDistance = (trim: number): number => …;

// src/world/approach.ts:26-26 variable MIN_LINK_LENGTH
/** Drivable length every link must retain between two approach zones. */
export const MIN_LINK_LENGTH = m(14.4);
