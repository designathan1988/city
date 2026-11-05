// What the feature uses of src/world/economy.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/economy.ts:14-17 interface Economy
export interface Economy {
  /** Money in hand. Never negative through an edit: one that cannot be paid is refused. */
  readonly balance: number;
}

// src/world/economy.ts:19-19 variable DEFAULT_ECONOMY
export const DEFAULT_ECONOMY: Economy = { balance: ROAD_TUNING.economy.startingFunds };

// src/world/economy.ts:22-26 function readEconomy
/** The saved form, or null when it is not one (`editor/persistence.ts` refuses the map then). */
export function readEconomy(data: unknown): Economy | null;

// src/world/economy.ts:37-41 function pricePerUnit
/** Price of one world unit of road of this cross-section, built at grade. */
export function pricePerUnit(rt: RoadType): number;
