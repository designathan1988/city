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

// src/world/economy.ts:37-41 function pricePerUnit
/** Price of one world unit of road of this cross-section, built at grade. */
export function pricePerUnit(rt: RoadType): number;

// src/world/economy.ts:44-60 function buildLengths
/** How much of a road of `length` is built each way, from its structure and the authored height at its two ends */
export function buildLengths(structure: RoadStructure, heightA: number, heightB: number, length: number): Record<BuildKind, number>;

// src/world/economy.ts:63-70 function segmentCost
/** What one road costs to build, as it stands in `doc`, with its length `length` (world units). */
export function segmentCost(doc: RoadDoc, segment: RoadSegment, length: number): number;

// src/world/economy.ts:109-147 function roadValue
/** DO NOT price every road of the map for each edit and each preview of a draft */
export function roadValue(doc: RoadDoc): number;

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

// src/world/economy.ts:160-169 function chargeFor
/** The charge for turning roads worth `valueBefore` into roads worth `valueAfter` with `balance` in hand */
export function chargeFor(valueBefore: number, valueAfter: number, balance: number): RoadCharge;
