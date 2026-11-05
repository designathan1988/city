// What the feature uses of src/core/statechart.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/core/statechart.ts:22-28 interface TransitionRecord
export interface TransitionRecord {
  readonly serial: number;
  readonly from: string;
  readonly event: string;
  readonly to: string;
  readonly at: number;
}
