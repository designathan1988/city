// What the feature uses of src/editor/roads/economy.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/roads/economy.ts:5-8 interface RoadsBefore
/** Paying for a road edit (`world/economy.ts`) */
export interface RoadsBefore {
  readonly value: number;
  readonly balance: number;
}

// src/editor/roads/economy.ts:11-13 function roadsBefore
/** What the roads are worth and the money in hand, before an edit. */
export function roadsBefore(doc: RoadDoc): RoadsBefore;

// src/editor/roads/economy.ts:16-22 function settleRoadEdit
/** The charge for the edit `after` holds against `before`, written into `after` when it can be paid */
export function settleRoadEdit(before: RoadsBefore, after: RoadDoc, apply = true): RoadCharge;
