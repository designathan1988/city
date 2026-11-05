// What the feature uses of src/world/approach.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/approach.ts:5-5 variable CROSSWALK_SETBACK
/** The approach zone: what sits between a road and a junction mouth. */

/** Clear space between the junction mouth and the near edge of the zebra. */
const CROSSWALK_SETBACK = m(1.2);

// src/world/approach.ts:7-7 variable CROSSWALK_DEPTH
/** Depth of the zebra along the direction of travel: 3 m, the Brazilian minimum for an urban crossing */
export const CROSSWALK_DEPTH = m(3);

// src/world/approach.ts:9-9 variable STOP_BAR_SETBACK
/** Clear space between the far edge of the zebra and the stop line. */
export const STOP_BAR_SETBACK = m(0.64);

// src/world/approach.ts:11-11 variable STOP_BAR_WIDTH
/** Painted width of the stop bar itself. */
export const STOP_BAR_WIDTH = m(0.4);

// src/world/approach.ts:14-15 function crosswalkDistance
/** Distance from the node to the centre of the crossing. */
export const crosswalkDistance = (trim: number): number => …;

// src/world/approach.ts:22-23 variable APPROACH_DEPTH
/** How much further back than the junction mouth the approach zone reaches. */
const APPROACH_DEPTH =
  CROSSWALK_SETBACK + CROSSWALK_DEPTH + STOP_BAR_SETBACK;

// src/world/approach.ts:26-26 variable MIN_LINK_LENGTH
/** Drivable length every link must retain between two approach zones. */
export const MIN_LINK_LENGTH = m(14.4);

// src/world/approach.ts:29-29 variable MIN_DRIVABLE_RESERVE
/** Total length a segment must reserve for drivable road plus both approaches. */
export const MIN_DRIVABLE_RESERVE = MIN_LINK_LENGTH + 2 * APPROACH_DEPTH;

// src/world/approach.ts:34-34 variable LEG_TRIM_CAP
/** The fractional caps that stop one end of a segment eating the whole thing. */

/** Ceiling on one leg's own trim guess, before the solver reconciles legs. */
export const LEG_TRIM_CAP = 0.48;

// src/world/approach.ts:37-37 variable RESERVE_CAP
/** Ceiling on the drivable reserve a short segment is asked to keep. */
export const RESERVE_CAP = 0.86;

// src/world/approach.ts:40-40 variable STOP_LINE_CAP
/** Ceiling on the stop line's distance from a node. */
export const STOP_LINE_CAP = 0.45;

// src/world/approach.ts:43-43 variable CROSSWALK_CAP
/** Ceiling on the crossing's distance from a node. */
export const CROSSWALK_CAP = 0.49;
