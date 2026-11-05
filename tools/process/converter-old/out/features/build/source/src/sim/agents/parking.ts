// The code of src/sim/agents/parking.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/parking.ts:73-81 slice of src/sim/agents/parking.ts#BakedBays (shared): the feature's statements; "…" marks code left out
export interface BakedBays {
  // … 1 lines
  lots(): Iterable<BuildingId>;
  // … 2 lines
  lot(id: BuildingId, record: number, sources: number): readonly Bay[] | null;
// … 2 lines
}

// src/sim/agents/parking.ts:92-123 slice of src/sim/agents/parking.ts#BayBook (shared): the feature's statements; "…" marks code left out
interface BayBook {
  // … 12 lines
  readonly known: Map<BuildingId, LotBays>;
  // … 1 lines
  readonly waiting: Map<BuildingId, LotSources>;
  // … 9 lines
  readonly bakedLots: Map<BuildingId, BakedBays>;
// … 5 lines
}

// src/sim/agents/parking.ts:230-230 slice of src/sim/agents/parking.ts#Asked (shared): the feature's statements; "…" marks code left out
interface Asked { readonly id: BuildingId; readonly sources: LotSources; readonly ask: LotAsk }

// src/sim/agents/parking.ts:233-275 slice of src/sim/agents/parking.ts#baysNow (shared): the feature's statements; "…" marks code left out
function baysNow(w: BayWorld): BayBook {
  // … 29 lines
  const look = rects === null ? book.lots.ids : lotsReached(w, book.lots, rects, delta!.ids);
  for (const id of look) lookAtLot(w, book, w.buildings.get(id)!, rects, walls, workers ? asked : null);
// … 10 lines
}

// src/sim/agents/parking.ts:335-341 slice of src/sim/agents/parking.ts#baySources (shared): the feature's statements; "…" marks code left out
export function baySources(w: BayWorld): string[] {
  // … 2 lines
  for (const id of book.lots.ids) if (book.known.has(id)) out.push(`L${id}`);
// … 2 lines
}

// src/sim/agents/parking.ts:354-359 slice of src/sim/agents/parking.ts#baysOfSource (shared): the feature's statements; "…" marks code left out
export function baysOfSource(w: BayWorld, source: string): Bay[] {
  // … 2 lines
  const bays = source[0] === 'L' ? book.known.get(id as BuildingId)?.bays : book.kerb.bays.get(id as SegmentId);
  return (bays ?? []).map((bay) => ({ ...bay, car: null }));
}

// src/sim/agents/parking.ts:362-368 slice of src/sim/agents/parking.ts#lotsReached (shared): the feature's statements; "…" marks code left out
function lotsReached(w: BayWorld, lots: BuildingsInOrder, rects: readonly ChangeRect[], named: readonly BuildingId[]): Set<BuildingId> {
  const out = new Set<BuildingId>();
  for (const id of named) if (lots.has(id)) out.add(id);
  // … 1 lines
  for (const id of w.buildings.nearAny(rects, LOT_REACHED, (b, r) => lots.has(b.id) && meets(r, b.reach))) out.add(id);
  return out;
}

// src/sim/agents/parking.ts:443-455 slice of src/sim/agents/parking.ts#buildingWalls (shared): the feature's statements; "…" marks code left out
function buildingWalls(store: TrafficBuildings, id: BuildingId): readonly Wall[] {
  // … 1 lines
  for (const ring of store.footprints(id)) {
    let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
    const digest = new Digest().add(id);
    for (const p of ring) {
      x0 = Math.min(x0, p.x); y0 = Math.min(y0, p.y); x1 = Math.max(x1, p.x); y1 = Math.max(y1, p.y);
      digest.add(p.x).add(p.y);
    }
    made.push({ building: id, ring, x0, y0, x1, y1, digest: digest.value() });
  }
// … 1 lines
}

// src/sim/agents/parking.ts:460-460 slice of src/sim/agents/parking.ts#Walls.read (shared): the feature's statements; "…" marks code left out
  private readonly read = new Map<BuildingId, readonly Wall[]>();
