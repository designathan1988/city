// The code of src/world/economy.ts that is the feature's (the rest of the file is shared).

// src/world/economy.ts:19-19 slice of src/world/economy.ts#DEFAULT_ECONOMY (shared): the feature's statements; "…" marks code left out
export const DEFAULT_ECONOMY: Economy = { balance: ROAD_TUNING.economy.startingFunds };

// src/world/economy.ts:28-28 slice of src/world/economy.ts#E (shared): the feature's statements; "…" marks code left out
const E = ROAD_TUNING.economy;

// src/world/economy.ts:30-34 slice of src/world/economy.ts#PER_SQUARE_UNIT (shared): the feature's statements; "…" marks code left out
const PER_SQUARE_UNIT = {
  carriageway: E.perSquareMetre.carriageway * METERS_PER_UNIT * METERS_PER_UNIT,
  median: E.perSquareMetre.median * METERS_PER_UNIT * METERS_PER_UNIT,
  footway: E.perSquareMetre.footway * METERS_PER_UNIT * METERS_PER_UNIT,
};

// src/world/economy.ts:37-41 slice of src/world/economy.ts#pricePerUnit (shared): the feature's statements; "…" marks code left out
export function pricePerUnit(rt: RoadType): number {
  // … 1 lines
  return carriageway * PER_SQUARE_UNIT.carriageway + rt.median * PER_SQUARE_UNIT.median +
    2 * rt.sidewalk * PER_SQUARE_UNIT.footway;
}

// src/world/economy.ts:44-60 slice of src/world/economy.ts#buildLengths (shared): the feature's statements; "…" marks code left out
export function buildLengths(structure: RoadStructure, heightA: number, heightB: number, length: number): Record<BuildKind, number> {
  // … 1 lines
  if (structure === 'tunnel') { out.tunnel = length; return out; }
  if (structure === 'elevated' || structure === 'bridge') { out.bridge = length; return out; }
  const fill = E.fillFrom, piers = ROAD_TUNING.structure.pierFrom;
  // … 6 lines
  out.trench = length * share(-Infinity, -fill);
  out.ground = length * share(-fill, fill);
  out.embankment = length * share(fill, piers);
  out.bridge = length * share(piers, Infinity);
// … 1 lines
}

// src/world/economy.ts:63-70 slice of src/world/economy.ts#segmentCost (shared): the feature's statements; "…" marks code left out
export function segmentCost(doc: RoadDoc, segment: RoadSegment, length: number): number {
  // … 4 lines
  for (const kind of BUILD_KINDS) total += lengths[kind] * unit * E.structure[kind];
  return total;
}

// src/world/economy.ts:160-169 slice of src/world/economy.ts#chargeFor (shared): the feature's statements; "…" marks code left out
export function chargeFor(valueBefore: number, valueAfter: number, balance: number): RoadCharge {
  // … 6 lines
  const refund = Math.round(-delta * E.demolitionRefund);
  return { amount: -refund, balance: balance + refund, affordable: true };
}
