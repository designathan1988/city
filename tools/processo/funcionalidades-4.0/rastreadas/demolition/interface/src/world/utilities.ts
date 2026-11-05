// What the feature uses of src/world/utilities.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/utilities.ts:9-9 variable POLE_HEIGHT
/** The overhead utility network: poles, the wires strung between them, and the lamps they carry. */

/** Height of a pole's mast, from the ground to the crown. */
export const POLE_HEIGHT = m(8.5);

// src/world/utilities.ts:14-14 variable POLE_ARM_HALF
/** Half-length of the cross-arm the wires are strung from. */
export const POLE_ARM_HALF = m(0.85);

// src/world/utilities.ts:16-16 variable POLE_ARM_DROP
/** How far below the crown the cross-arm sits. */
export const POLE_ARM_DROP = m(0.55);

// src/world/utilities.ts:24-24 variable POLE_LAMP_REACH
/** Lamp arm reach and head, for the poles that carry a light. */
export const POLE_LAMP_REACH = m(1.6);

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
