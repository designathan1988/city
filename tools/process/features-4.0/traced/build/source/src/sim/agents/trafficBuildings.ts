// The code of src/sim/agents/trafficBuildings.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/trafficBuildings.ts:28-28 slice of src/sim/agents/trafficBuildings.ts#TrafficBlock (shared): the feature's statements; "…" marks code left out
type TrafficBlock = Pick<Volume, 'x' | 'y' | 'w' | 'd' | 'open' | 'base'>;

// src/sim/agents/trafficBuildings.ts:30-30 slice of src/sim/agents/trafficBuildings.ts#TrafficPart (shared): the feature's statements; "…" marks code left out
export type TrafficPart = LotPart & Pick<BuildingElement, 'id'>;

// src/sim/agents/trafficBuildings.ts:33-55 slice of src/sim/agents/trafficBuildings.ts#TrafficBuilding (shared): the feature's statements; "…" marks code left out
export interface TrafficBuilding {
  readonly id: BuildingId;
  // … 3 lines
  readonly use: BuildingUse;
  readonly function?: BuildingFunction;
  // … 10 lines
  readonly doors: readonly StreetDoor[];
  // … 1 lines
  readonly docks: readonly LorryDock[];
// … 2 lines
}

// src/sim/agents/trafficBuildings.ts:59-61 slice of src/sim/agents/trafficBuildings.ts#partOf (shared): the feature's statements; "…" marks code left out
function partOf(el: BuildingElement): TrafficPart {
  return { id: el.id, kind: el.kind, x: el.x, y: el.y, w: el.w, d: el.d, facing: el.facing, z: el.z, ...(el.angle !== undefined ? { angle: el.angle } : {}) };
}

// src/sim/agents/trafficBuildings.ts:63-76 slice of src/sim/agents/trafficBuildings.ts#project (shared): the feature's statements; "…" marks code left out
function project(b: Building): TrafficBuilding {
  return {
    id: b.id, x: b.x, y: b.y, rotation: b.rotation, use: b.use,
    ...(b.function !== undefined ? { function: b.function } : {}),
    volumes: b.volumes.map((v) => ({ x: v.x, y: v.y, w: v.w, d: v.d, base: v.base, ...(v.open !== undefined ? { open: v.open } : {}) })),
    ...(b.elements ? { elements: b.elements.map(partOf) } : {}),
    bounds: boxOf(storedBounds(b)),
    reach: boxOf(storedBounds(b, LOT_READS)),
    footprints: solidFootprints(b),
    doors: streetDoors(b),
    docks: lorryDocks(b),
    entrances: entrancesOf(b),
  };
}

// src/sim/agents/trafficBuildings.ts:79-79 slice of src/sim/agents/trafficBuildings.ts#trafficBuilding (shared): the feature's statements; "…" marks code left out
export const trafficBuilding: (b: Building) => TrafficBuilding = project;

// src/sim/agents/trafficBuildings.ts:91-102 slice of src/sim/agents/trafficBuildings.ts#TrafficFacts (shared): the feature's statements; "…" marks code left out
export interface TrafficFacts {
  readonly id: BuildingId;
// … 9 lines
}

// src/sim/agents/trafficBuildings.ts:105-130 slice of src/sim/agents/trafficBuildings.ts#frontageBound (shared): the feature's statements; "…" marks code left out
export function frontageBound(b: Pick<TrafficBuilding, 'volumes' | 'elements'>): number {
  // … 1 lines
  if (!gates.length) {
    // … 1 lines
    for (const v of b.volumes) { lo = Math.min(lo, v.x); hi = Math.max(hi, v.x + v.w); }
    return Math.max(m(6), hi - lo);
  }
  // … 1 lines
  for (const g of gates) {
    const alongY = g.facing === 1 || g.facing === 3;
    let ux = alongY ? 0 : 1, uy = alongY ? 1 : 0;
    if (g.angle) {
      const c = Math.cos(g.angle), s = Math.sin(g.angle);
      [ux, uy] = [ux * c - uy * s, ux * s + uy * c];
    }
    // … 1 lines
    for (const v of b.volumes) {
      for (const [x, y] of [[v.x, v.y], [v.x + v.w, v.y], [v.x, v.y + v.d], [v.x + v.w, v.y + v.d]] as const) {
        const a = (x - g.x) * ux + (y - g.y) * uy;
        lo = Math.min(lo, a); hi = Math.max(hi, a);
      }
    }
    best = Math.max(best, Math.max(m(6), hi - lo));
  }
  return best;
}

// src/sim/agents/trafficBuildings.ts:132-133 slice of src/sim/agents/trafficBuildings.ts#traitsOf (shared): the feature's statements; "…" marks code left out
const traitsOf = (b: TrafficBuilding): number =>
  ((b.elements ?? []).some((el) => el.kind === 'parking') ? TRAIT_PARKING : 0) | (gatedProperty(b) !== null && b.docks.length > 0 ? TRAIT_YARD : 0);

// src/sim/agents/trafficBuildings.ts:141-165 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildings (shared): the feature's statements; "…" marks code left out
export interface TrafficBuildings {
  // … 1 lines
  get(id: BuildingId): TrafficBuilding | undefined;
  // … 3 lines
  ids(): Iterable<BuildingId>;
  // … 1 lines
  facts(id: BuildingId): TrafficFacts | undefined;
  // … 1 lines
  entrances(id: BuildingId): readonly [Vec2, Vec2][];
  // … 1 lines
  footprints(id: BuildingId): readonly (readonly Vec2[])[];
  // … 2 lines
  eachEntrances(visit: (id: BuildingId, entrances: readonly [Vec2, Vec2][]) => void): void;
  eachFootprints(visit: (id: BuildingId, footprints: readonly (readonly Vec2[])[]) => void): void;
  // … 3 lines
  nearIds(minX: number, minY: number, maxX: number, maxY: number): BuildingId[];
  // … 1 lines
  nearAny(rects: readonly (readonly [number, number, number, number])[], pad: number,
    meets: (b: TrafficFacts, rect: readonly [number, number, number, number]) => boolean): BuildingId[];
}

// src/sim/agents/trafficBuildings.ts:174-220 slice of src/sim/agents/trafficBuildings.ts#docBuildings (shared): the feature's statements; "…" marks code left out
export function docBuildings(doc: RoadDoc): TrafficBuildings {
  // … 1 lines
  const known = new WeakMap<Building, TrafficBuilding>();
// … 43 lines
}

// src/sim/agents/trafficBuildings.ts:177-181 slice of src/sim/agents/trafficBuildings.ts#docBuildings.trafficBuilding (shared): the feature's statements; "…" marks code left out
  const trafficBuilding = (b: Building): TrafficBuilding => {
    let t = known.get(b);
    if (!t) known.set(b, t = project(b));
    return t;
  };

// src/sim/agents/trafficBuildings.ts:183-188 slice of src/sim/agents/trafficBuildings.ts#docBuildings.factsOfRecord (shared): the feature's statements; "…" marks code left out
  const factsOfRecord = (b: Building): TrafficFacts => {
    const t = trafficBuilding(b);
    let f = facts.get(t);
    if (!f) facts.set(t, f = factsOf(t));
    return f;
  };

// src/sim/agents/trafficBuildings.ts:190-193 slice of src/sim/agents/trafficBuildings.ts#docBuildings.get (shared): the feature's statements; "…" marks code left out
    get: (id) => {
      const b = doc.buildings.get(id);
      return b ? trafficBuilding(b) : undefined;
    },

// src/sim/agents/trafficBuildings.ts:197-197 slice of src/sim/agents/trafficBuildings.ts#docBuildings.ids (shared): the feature's statements; "…" marks code left out
    ids: () => doc.buildings.idList(),

// src/sim/agents/trafficBuildings.ts:198-201 slice of src/sim/agents/trafficBuildings.ts#docBuildings.facts (shared): the feature's statements; "…" marks code left out
    facts: (id) => {
      const b = doc.buildings.get(id);
      return b ? factsOfRecord(b) : undefined;
    },

// src/sim/agents/trafficBuildings.ts:202-205 slice of src/sim/agents/trafficBuildings.ts#docBuildings.entrances (shared): the feature's statements; "…" marks code left out
    entrances: (id) => {
      const b = doc.buildings.get(id);
      return b ? trafficBuilding(b).entrances : [];
    },

// src/sim/agents/trafficBuildings.ts:206-209 slice of src/sim/agents/trafficBuildings.ts#docBuildings.footprints (shared): the feature's statements; "…" marks code left out
    footprints: (id) => {
      const b = doc.buildings.get(id);
      return b ? trafficBuilding(b).footprints : [];
    },

// src/sim/agents/trafficBuildings.ts:211-211 slice of src/sim/agents/trafficBuildings.ts#docBuildings.eachFacts (shared): the feature's statements; "…" marks code left out
    eachFacts: (visit) => { for (const b of doc.buildings.all()) visit(factsOfRecord(b)); },

// src/sim/agents/trafficBuildings.ts:213-213 slice of src/sim/agents/trafficBuildings.ts#docBuildings.eachEntrances (shared): the feature's statements; "…" marks code left out
    eachEntrances: (visit) => { for (const b of doc.buildings.all()) visit(b.id, trafficBuilding(b).entrances); },

// src/sim/agents/trafficBuildings.ts:215-215 slice of src/sim/agents/trafficBuildings.ts#docBuildings.eachFootprints (shared): the feature's statements; "…" marks code left out
    eachFootprints: (visit) => { for (const b of doc.buildings.all()) visit(b.id, trafficBuilding(b).footprints); },

// src/sim/agents/trafficBuildings.ts:216-216 slice of src/sim/agents/trafficBuildings.ts#docBuildings.near (shared): the feature's statements; "…" marks code left out
    near: (minX, minY, maxX, maxY) => doc.buildings.near(minX, minY, maxX, maxY).map(trafficBuilding),

// src/sim/agents/trafficBuildings.ts:217-217 slice of src/sim/agents/trafficBuildings.ts#docBuildings.nearIds (shared): the feature's statements; "…" marks code left out
    nearIds: (minX, minY, maxX, maxY) => doc.buildings.near(minX, minY, maxX, maxY).map((b) => b.id),

// src/sim/agents/trafficBuildings.ts:218-218 slice of src/sim/agents/trafficBuildings.ts#docBuildings.nearAny (shared): the feature's statements; "…" marks code left out
    nearAny: (rects, pad, meets) => doc.buildings.nearAny(rects, pad, (b, r) => meets(factsOfRecord(b), r)).map((b) => b.id),

// src/sim/agents/trafficBuildings.ts:229-233 slice of src/sim/agents/trafficBuildings.ts#BuildingChange (shared): the feature's statements; "…" marks code left out
export interface BuildingChange {
  readonly id: BuildingId;
  // … 1 lines
  readonly ref: RecordRef | null;
}

// src/sim/agents/trafficBuildings.ts:285-285 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.readers (shared): the feature's statements; "…" marks code left out
  private readers: (TableReader | null)[] = [];

// src/sim/agents/trafficBuildings.ts:306-309 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.get (shared): the feature's statements; "…" marks code left out
  get(id: BuildingId): TrafficBuilding | undefined {
    const slot = this.slots.get(id);
    return slot < 0 ? undefined : this.record(slot);
  }

// src/sim/agents/trafficBuildings.ts:315-320 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.ids (shared): the feature's statements; "…" marks code left out
  *ids(): IterableIterator<BuildingId> {
    for (let i = 0; i < this.orderLength; i++) {
      // … 1 lines
      if (!Number.isNaN(id)) yield id as BuildingId;
    }
  }

// src/sim/agents/trafficBuildings.ts:322-325 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.facts (shared): the feature's statements; "…" marks code left out
  facts(id: BuildingId): TrafficFacts | undefined {
    const slot = this.slots.get(id);
    return slot < 0 ? undefined : this.factsAt(slot);
  }

// src/sim/agents/trafficBuildings.ts:327-333 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.entrances (shared): the feature's statements; "…" marks code left out
  entrances(id: BuildingId): readonly [Vec2, Vec2][] {
    const slot = this.slots.get(id);
    const runs = this.entranceRuns, n = slot < 0 ? 0 : runs.length(slot);
    // … 1 lines
    for (let at = runs.start(slot), end = at + n, d = runs.data; at < end; at += 4) out.push([{ x: d[at]!, y: d[at + 1]! }, { x: d[at + 2]!, y: d[at + 3]! }]);
  // … 1 lines
  }

// src/sim/agents/trafficBuildings.ts:339-341 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachEntrances (shared): the feature's statements; "…" marks code left out
  eachEntrances(visit: (id: BuildingId, entrances: readonly [Vec2, Vec2][]) => void): void {
    for (const id of this.ids()) visit(id, this.entrances(id));
  }

// src/sim/agents/trafficBuildings.ts:343-345 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.eachFootprints (shared): the feature's statements; "…" marks code left out
  eachFootprints(visit: (id: BuildingId, footprints: readonly (readonly Vec2[])[]) => void): void {
    for (const id of this.ids()) visit(id, this.footprints(id));
  }

// src/sim/agents/trafficBuildings.ts:347-360 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.footprints (shared): the feature's statements; "…" marks code left out
  footprints(id: BuildingId): readonly (readonly Vec2[])[] {
    const slot = this.slots.get(id);
    // … 1 lines
    if (slot < 0 || !runs.length(slot)) return [];
    // … 1 lines
    let at = runs.start(slot);
    // … 1 lines
    for (let r = 0, rings = d[at++]!; r < rings; r++) {
      const ring: Vec2[] = [];
      for (let k = 0, n = d[at++]!; k < n; k++, at += 2) ring.push({ x: d[at]!, y: d[at + 1]! });
      out.push(ring);
    }
  // … 1 lines
  }

// src/sim/agents/trafficBuildings.ts:363-388 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.load (shared): the feature's statements; "…" marks code left out
  load(sections: BuildingSections): void {
    // … 5 lines
    this.order = new Float64Array(Math.max(16, sectionsSize(sections)));
    // … 6 lines
    const size = sectionsSize(sections);
    this.grow(size);
    this.entranceRuns.reserve(size);
    this.footprintRuns.reserve(size);
    // … 1 lines
    const reading = new Map<ArrayBufferLike, TableReader>();
    for (const ref of eachSectionRef(sections)) {
      let reader = reading.get(ref.buffer);
      if (!reader) reading.set(ref.buffer, reader = new RecordTable(ref.buffer).reader());
      this.place(project(reader.record(ref.row)), ref);
    }
  // … 1 lines
  }

// src/sim/agents/trafficBuildings.ts:391-412 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.apply (shared): the feature's statements; "…" marks code left out
  apply(changes: readonly BuildingChange[]): void {
    const ids: BuildingId[] = [];
    // … 2 lines
    for (const { id, value, ref } of changes) {
      // … 1 lines
      if (value === null || ref === null) {
        // … 3 lines
        ids.push(id); ops += BATCH_GONE;
      // … 1 lines
      }
      // … 3 lines
      ids.push(id);
    // … 2 lines
    }
    if (ids.length) this.journal.record('buildings', rects, { ids, detail: `${BATCH_DETAIL}${ops}` });
  }

// src/sim/agents/trafficBuildings.ts:418-420 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.nearIds (shared): the feature's statements; "…" marks code left out
  nearIds(minX: number, minY: number, maxX: number, maxY: number): BuildingId[] {
    return this.inOrder(this.meeting([minX, minY, maxX, maxY])).map((slot) => this.idAt[slot]! as BuildingId);
  }

// src/sim/agents/trafficBuildings.ts:422-431 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.nearAny (shared): the feature's statements; "…" marks code left out
  nearAny(rects: readonly (readonly [number, number, number, number])[], pad: number,
    meets: (b: TrafficFacts, rect: readonly [number, number, number, number]) => boolean): BuildingId[] {
    // … 6 lines
    return this.inOrder([...kept]).map((slot) => this.idAt[slot]! as BuildingId);
  }

// src/sim/agents/trafficBuildings.ts:448-456 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.factsAt (shared): the feature's statements; "…" marks code left out
  private factsAt(slot: number): TrafficFacts {
    // … 1 lines
    return {
      id: this.idAt[slot]! as BuildingId, x: this.xs[slot]!, y: this.ys[slot]!, kind: TRAFFIC_KINDS[this.kinds[slot]!]!, traits: this.traits[slot]!,
    // … 3 lines
    };
  }

// src/sim/agents/trafficBuildings.ts:459-487 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.place (shared): the feature's statements; "…" marks code left out
  private place(b: TrafficBuilding, ref: RecordRef): number {
    // … 24 lines
    this.keepRef(slot, ref);
  // … 2 lines
  }

// src/sim/agents/trafficBuildings.ts:489-499 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.remove (shared): the feature's statements; "…" marks code left out
  private remove(id: BuildingId, slot: number): void {
    // … 8 lines
    this.slots.delete(id);
  }

// src/sim/agents/trafficBuildings.ts:550-560 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.keepRef (shared): the feature's statements; "…" marks code left out
  private keepRef(slot: number, ref: RecordRef): void {
    let at = this.bufferIndex.get(ref.buffer);
    if (at === undefined) {
      at = this.freeBuffers.length ? this.freeBuffers.pop()! : this.buffers.length;
      this.buffers[at] = ref.buffer; this.bufferUses[at] = 0; this.readers[at] = null;
      this.bufferIndex.set(ref.buffer, at);
    }
    this.bufferUses[at]!++;
    this.bufferAt[slot] = at;
    this.rows[slot] = ref.row;
  }

// src/sim/agents/trafficBuildings.ts:573-587 slice of src/sim/agents/trafficBuildings.ts#TrafficBuildingTable.record (shared): the feature's statements; "…" marks code left out
  private record(slot: number): TrafficBuilding {
    // … 3 lines
    if (!b) {
      const at = this.bufferAt[slot]!;
      // A weak reader (`TableReader`): the members it decoded live only while a record read holds them.
      const reader = this.readers[at] ??= new RecordTable(this.buffers[at]!).reader(true);
      b = project(reader.record(this.rows[slot]!));
      this.alive.set(slot, new WeakRef(b));
      this.gone.register(b, slot);
    }
    this.hold(slot, b);
    return b;
  }

// src/sim/agents/trafficBuildings.ts:692-695 slice of src/sim/agents/trafficBuildings.ts#BuildingSent (shared): the feature's statements; "…" marks code left out
export interface BuildingSent {
  readonly id: BuildingId;
  readonly ref: RecordRef | null;
}

// src/sim/agents/trafficBuildings.ts:698-703 slice of src/sim/agents/trafficBuildings.ts#TrafficMap (shared): the feature's statements; "…" marks code left out
export interface TrafficMap {
  // … 1 lines
  readonly lots: readonly Lot[];
  // … 1 lines
  readonly buildings: BuildingSections;
}

// src/sim/agents/trafficBuildings.ts:706-719 slice of src/sim/agents/trafficBuildings.ts#trafficMapOf (shared): the feature's statements; "…" marks code left out
export function trafficMapOf(doc: RoadDoc): TrafficMap {
  return {
    // … 6 lines
    lots: doc.lotStore.toJSON(),
    lotKeys: [...doc.lotStore.keys],
    // … 1 lines
    buildings: doc.buildings.sections(),
  };
}

// src/sim/agents/trafficBuildings.ts:722-729 slice of src/sim/agents/trafficBuildings.ts#buildingChangesOf (shared): the feature's statements; "…" marks code left out
export function buildingChangesOf(doc: RoadDoc, ids: Iterable<BuildingId>): BuildingSent[] {
  // … 1 lines
  for (const id of ids) {
    const b = doc.buildings.get(id);
    out.push({ id, ref: b ? refOf(b, false) : null });
  }
// … 1 lines
}

// src/sim/agents/trafficBuildings.ts:732-738 slice of src/sim/agents/trafficBuildings.ts#trafficChangesOf (shared): the feature's statements; "…" marks code left out
export function trafficChangesOf(sent: readonly BuildingSent[]): BuildingChange[] {
  // … 1 lines
  const read = eachRecord(there.map((s) => s.ref!));
  // … 1 lines
  for (const s of there) values.set(s, project(read.next().value as Building));
// … 1 lines
}

// src/sim/agents/trafficBuildings.ts:741-746 slice of src/sim/agents/trafficBuildings.ts#takeTrafficMap (shared): the feature's statements; "…" marks code left out
export function takeTrafficMap(doc: RoadDoc, buildings: TrafficBuildingTable, map: TrafficMap): void {
  // … 2 lines
  doc.lotStore.load(map.lots, map.lotKeys);
// … 1 lines
}
