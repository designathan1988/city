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

// src/world/economy.ts:37-41 function pricePerUnit: the feature's own
export function pricePerUnit(rt: RoadType): number {
  const carriageway = Math.max(0, rt.width - rt.median);
  return carriageway * PER_SQUARE_UNIT.carriageway + rt.median * PER_SQUARE_UNIT.median +
    2 * rt.sidewalk * PER_SQUARE_UNIT.footway;
}

// src/world/economy.ts:44-60 function buildLengths: the feature's own
export function buildLengths(structure: RoadStructure, heightA: number, heightB: number, length: number): Record<BuildKind, number> {
  const out: Record<BuildKind, number> = { ground: 0, embankment: 0, trench: 0, bridge: 0, tunnel: 0 };
  if (structure === 'tunnel') { out.tunnel = length; return out; }
  if (structure === 'elevated' || structure === 'bridge') { out.bridge = length; return out; }
  const fill = E.fillFrom, piers = ROAD_TUNING.structure.pierFrom;
  /** Share of the road whose height lies in [lo, hi). */
  const share = (lo: number, hi: number): number => {
    if (heightA === heightB) return heightA >= lo && heightA < hi ? 1 : 0;
    const t0 = (lo - heightA) / (heightB - heightA), t1 = (hi - heightA) / (heightB - heightA);
    return Math.max(0, Math.min(1, Math.max(t0, t1)) - Math.max(0, Math.min(t0, t1)));
  };
  out.trench = length * share(-Infinity, -fill);
  out.ground = length * share(-fill, fill);
  out.embankment = length * share(fill, piers);
  out.bridge = length * share(piers, Infinity);
  return out;
}

// src/world/economy.ts:63-70 function segmentCost: the feature's own
export function segmentCost(doc: RoadDoc, segment: RoadSegment, length: number): number {
  const rt = roadProfile(segment.type, segment.lanes, segment.direction, segment.section, segment.parking);
  const lengths = buildLengths(segment.structure, doc.node(segment.a)?.heightOffset ?? 0, doc.node(segment.b)?.heightOffset ?? 0, length);
  const unit = pricePerUnit(rt);
  let total = 0;
  for (const kind of BUILD_KINDS) total += lengths[kind] * unit * E.structure[kind];
  return total;
}

// src/world/economy.ts:86-92 function priceOf: the feature's own
function priceOf(ledger: Ledger, id: SegmentId): number {
  for (let at: Ledger | null = ledger; at; at = at.base) {
    const cost = at.costs.get(id);
    if (cost !== undefined) return cost;
  }
  return 0;
}

// src/world/economy.ts:94-94 function segmentPrice: the feature's own
const segmentPrice = (doc: RoadDoc, s: RoadSegment): number => segmentCost(doc, s, segmentPolyline(doc, s).length);

// src/world/economy.ts:97-106 function priceAll: the feature's own
function priceAll(doc: RoadDoc): Ledger {
  const costs = new Map<SegmentId, number>();
  let total = 0;
  for (const segment of doc.segments.values()) {
    const cost = segmentPrice(doc, segment);
    costs.set(segment.id, cost);
    total += cost;
  }
  return { journal: doc.changes, seen: doc.changes.version, costs, base: null, total };
}

// src/world/economy.ts:109-147 function roadValue: the feature's own
export function roadValue(doc: RoadDoc): number {
  let ledger = ledgers.get(doc);
  if (!ledger || ledger.journal !== doc.changes) {
    const origin = doc.forkOrigin;
    if (origin) {
      roadValue(origin);
      const base = ledgers.get(origin)!;
      ledger = { journal: doc.changes, seen: origin.changes.version, costs: new Map(), base, total: base.total };
    } else ledger = priceAll(doc);
    ledgers.set(doc, ledger);
  }
  // A copy whose original moved on since it was made reads the original as it is now: priced whole instead.
  if (ledger.base && !doc.forkOrigin) { ledger = priceAll(doc); ledgers.set(doc, ledger); }
  if (ledger.seen === doc.changes.version) return ledger.total;
  const entries = doc.changes.since(ledger.seen, ['roads', 'traffic']);
  if (entries === null || entries.some((e) => !e.ids)) {
    ledger = priceAll(doc);
    ledgers.set(doc, ledger);
    return ledger.total;
  }
  const dirty = new Set<SegmentId>();
  for (const entry of entries) {
    for (const id of entry.ids!) {
      // An id may be a road's or a node's (they share the roads' entries): both read.
      dirty.add(id as SegmentId);
      for (const s of doc.node(id as never)?.incident ?? []) dirty.add(s);
    }
  }
  for (const id of dirty) {
    const segment = doc.segment(id);
    const was = priceOf(ledger, id);
    const now = segment ? segmentPrice(doc, segment) : 0;
    if (now === was) continue;
    ledger.costs.set(id, now);
    ledger.total += now - was;
  }
  ledger.seen = doc.changes.version;
  return ledger.total;
}

// src/world/economy.ts:150-157 interface RoadCharge: the feature's own
export interface RoadCharge {
  /** Money taken (positive) or given back (negative). */
  readonly amount: number;
  /** The balance after the edit. */
  readonly balance: number;
  /** False when the edit costs more than the balance holds: it is refused. */
  readonly affordable: boolean;
}

// src/world/economy.ts:160-169 function chargeFor: the feature's own
export function chargeFor(valueBefore: number, valueAfter: number, balance: number): RoadCharge {
  const delta = valueAfter - valueBefore;
  if (Math.abs(delta) < 1) return { amount: 0, balance, affordable: true };
  if (delta > 0) {
    const amount = Math.round(delta);
    return { amount, balance: balance - amount, affordable: amount <= balance };
  }
  const refund = Math.round(-delta * E.demolitionRefund);
  return { amount: -refund, balance: balance + refund, affordable: true };
}
