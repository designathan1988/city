// What the feature uses of src/world/utilities.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/utilities.ts:43-49 interface UtilityPole
export interface UtilityPole {
  readonly id: PoleId;
  x: number;
  y: number;
  /** Whether this pole carries a street light. */
  lamp: boolean;
}
