// What the feature uses of src/world/conflictTable.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/conflictTable.ts:15-15 type BodyClass
/** WHERE TWO MOVEMENTS' BODIES MEET, measured from their sampled sweeps alone */

/** Size classes a conflict zone is measured for. */
export type BodyClass = 0 | 1 | 2;

// src/world/conflictTable.ts:45-49 variable BODY_ENVELOPE
export const BODY_ENVELOPE: readonly { readonly length: number; readonly width: number }[] = [
  { length: m(2.2), width: m(0.9) },
  { length: m(5.7), width: m(2.05) },
  { length: m(12.1), width: m(2.6) },
];

// src/world/conflictTable.ts:50-50 variable HEAVY
export const HEAVY: BodyClass = 2;

// src/world/conflictTable.ts:447-447 type TurnKind
/** DO NOT define them in `junctionBuild.ts` */

/** How a movement turns (`junctionBuild.ts` `classifyTurn`). */
export type TurnKind = 'through' | 'left' | 'right' | 'uturn';
