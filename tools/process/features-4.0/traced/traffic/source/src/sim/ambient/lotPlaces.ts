// The code of src/sim/ambient/lotPlaces.ts that is the feature's (the rest of the file is shared).

// src/sim/ambient/lotPlaces.ts:80-82 slice of src/sim/ambient/lotPlaces.ts#PlaceDeps (shared): the feature's statements; "…" marks code left out
export interface PlaceDeps<W extends PlaceWay> {
  readonly wayOf: (b: TrafficBuilding) => W | null;
}

// src/sim/ambient/lotPlaces.ts:102-102 slice of src/sim/ambient/lotPlaces.ts#LotPlaces.store (shared): the feature's statements; "…" marks code left out
  private store: TrafficBuildings | null = null;

// src/sim/ambient/lotPlaces.ts:133-179 slice of src/sim/ambient/lotPlaces.ts#LotPlaces.sync (shared): the feature's statements; "…" marks code left out
  sync(store: TrafficBuildings, journal: ChangeJournal): void {
    this.store = store;
    // … 3 lines
    if (!entries) { this.fileAll(store); return; }
    // … 3 lines
    for (const entry of entries) {
      if (entry.kind === 'buildings') {
        if (!entry.ids) { this.fileAll(store); return; }
      // … 1 lines
      } else if (!entry.rects) everyRoad = true;
      else rects.push(...entry.rects);
    }
    for (const id of ids) {
      // … 1 lines
      const b = store.facts(id as never);
      if (b) this.file(b);
    }
  // … 25 lines
  }

// src/sim/ambient/lotPlaces.ts:291-306 slice of src/sim/ambient/lotPlaces.ts#LotPlaces.workOut (shared): the feature's statements; "…" marks code left out
  private workOut(p: number, joins: (p: { x: number; y: number }) => boolean): void {
    // … 1 lines
    const b = this.store?.get(this.building[p]! as never);
    const way = b ? this.deps.wayOf(b) : null;
    const footless = !!way?.street && !joins(way.out[way.out.length - 1]!);
    const kept = footless ? null : way;
    this.ways[p] = kept;
    if (way?.street) this.streetish.add(p);
    const weight = kept ? weightOf(kept.frontage) : 0;
    if (weight > this.weight[p]!) ambientIndexWork.boundBroken++;
    if (kept) {
      const end = kept.out[kept.out.length - 1]!, at = p * 4, feet = this.feet;
      if (end.x < feet[at]! || end.x > feet[at + 2]! || end.y < feet[at + 1]! || end.y > feet[at + 3]!) ambientIndexWork.boundBroken++;
    }
    this.weigh(p, weight);
  }

// src/sim/ambient/lotPlaces.ts:327-331 slice of src/sim/ambient/lotPlaces.ts#LotPlaces.fileAll (shared): the feature's statements; "…" marks code left out
  private fileAll(store: TrafficBuildings): void {
    // … 2 lines
    store.eachFacts((b) => this.file(b));
  }

// src/sim/ambient/lotPlaces.ts:345-371 slice of src/sim/ambient/lotPlaces.ts#LotPlaces.file (shared): the feature's statements; "…" marks code left out
  private file(b: TrafficFacts): void {
    // … 1 lines
    const k = KIND_INDEX[b.kind];
    const bound = weightOf(b.frontage);
    const box = b.bounds;
    const placed = Number.isFinite(box.minX) && Number.isFinite(box.maxX) && Number.isFinite(box.minY) && Number.isFinite(box.maxY);
    const fx0 = box.minX - FEET_PAD, fy0 = box.minY - FEET_PAD, fx1 = box.maxX + FEET_PAD, fy1 = box.maxY + FEET_PAD;
    const cell = placed ? cellKey(Math.floor((fx0 + fx1) / 2 / PLACE_CELL), Math.floor((fy0 + fy1) / 2 / PLACE_CELL)) : null;
    const free = this.freeSlots[k]!;
    const slot = free.length ? free.pop()! : this.slots[k]!.length;
    // … 1 lines
    this.building[p] = b.id; this.kind[p] = k; this.bound[p] = bound; this.weight[p] = 0; this.slot[p] = slot;
    this.cell[p] = cell ?? Number.NaN; this.cellAt[p] = -1; this.ways[p] = undefined;
    // … 1 lines
    feet[at] = fx0; feet[at + 1] = fy0; feet[at + 2] = fx1; feet[at + 3] = fy1;
    this.slots[k]![slot] = p;
    this.byBuilding.set(b.id, p);
    if (cell !== null) {
      this.spread = Math.max(this.spread, (fx1 - fx0) / 2, (fy1 - fy0) / 2);
      let c = this.cells.get(cell);
      if (!c) this.cells.set(cell, c = { members: [], sums: [0, 0, 0, 0], x0: fx0, y0: fy0, x1: fx1, y1: fy1 });
      c.x0 = Math.min(c.x0, fx0); c.y0 = Math.min(c.y0, fy0); c.x1 = Math.max(c.x1, fx1); c.y1 = Math.max(c.y1, fy1);
      this.cellAt[p] = c.members.length;
      c.members.push(p);
    }
    this.weigh(p, bound);
  }
