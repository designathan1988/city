// What the feature uses of src/world/economy.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/economy.ts:11-11 type BuildKind
/** THE MINIMAL ECONOMY (the player's decision of 2026-10-09) */
export type BuildKind = 'ground' | 'embankment' | 'trench' | 'bridge' | 'tunnel';

// src/world/economy.ts:12-12 variable BUILD_KINDS
export const BUILD_KINDS: readonly BuildKind[] = ['ground', 'embankment', 'trench', 'bridge', 'tunnel'];

// src/world/economy.ts:14-17 interface Economy
export interface Economy {
  /** Money in hand. Never negative through an edit: one that cannot be paid is refused. */
  readonly balance: number;
}

// src/world/economy.ts:19-19 variable DEFAULT_ECONOMY
export const DEFAULT_ECONOMY: Economy = { balance: ROAD_TUNING.economy.startingFunds };

// src/world/economy.ts:28-28 variable E
const E = ROAD_TUNING.economy;

// src/world/economy.ts:30-34 variable PER_SQUARE_UNIT
/** Prices per square world unit, from the per-square-metre figures (a build-time conversion). */
const PER_SQUARE_UNIT = {
  carriageway: E.perSquareMetre.carriageway * METERS_PER_UNIT * METERS_PER_UNIT,
  median: E.perSquareMetre.median * METERS_PER_UNIT * METERS_PER_UNIT,
  footway: E.perSquareMetre.footway * METERS_PER_UNIT * METERS_PER_UNIT,
};

// src/world/economy.ts:73-82 interface Ledger
/** A document's road prices, kept from its diary: each road's price, and their sum kept by difference */
interface Ledger {
  readonly journal: ChangeJournal;
  /** The diary's serial read up to. */
  seen: number;
  /** Each road's price (0: gone), over the base's. */
  readonly costs: Map<SegmentId, number>;
  /** The original's ledger as it stood when the copy was made; null for a document's own. */
  readonly base: Ledger | null;
  total: number;
}

// src/world/economy.ts:84-84 variable ledgers
const ledgers = new WeakMap<RoadDoc, Ledger>();

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
