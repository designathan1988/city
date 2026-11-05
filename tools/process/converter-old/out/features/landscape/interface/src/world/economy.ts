// What the feature uses of src/world/economy.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/economy.ts:150-157 interface RoadCharge
/** What an edit costs and what the balance becomes. */
export interface RoadCharge {
  /** Money taken (positive) or given back (negative). */
  readonly amount: number;
  /** The balance after the edit. */
  readonly balance: number;
  /** False when the edit costs more than the balance holds: it is refused. */
  readonly affordable: boolean;
}
