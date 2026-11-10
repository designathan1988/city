// What the feature uses of src/sim/agents/bay.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/agents/bay.ts:7-18 interface BayLane
/** A parking bay and the lane it is reached from */

export interface BayLane {
  readonly lanelet: LaneletId;
  /** Arc position of the point where a car's body centre joins or leaves the lane. */
  readonly at: number;
  readonly x: number;
  readonly y: number;
  /** Unit direction of travel there. */
  readonly tx: number;
  readonly ty: number;
  /** For a lot entered by a car gate: the arc position */
  readonly entryAt?: number;
}
