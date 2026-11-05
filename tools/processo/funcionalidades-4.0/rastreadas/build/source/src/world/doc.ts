// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:289-289 slice of src/world/doc.ts#RoadDoc.buildings (shared): the feature's statements; "…" marks code left out
  readonly buildings = new BuildingStore(this.changes);

// src/world/doc.ts:292-292 slice of src/world/doc.ts#RoadDoc.zones (shared): the feature's statements; "…" marks code left out
  readonly zones: Zone[] = [];

// src/world/doc.ts:295-295 slice of src/world/doc.ts#RoadDoc.zoneMarks (shared): the feature's statements; "…" marks code left out
  readonly zoneMarks: ZoneMark[] = [];

// src/world/doc.ts:297-297 slice of src/world/doc.ts#RoadDoc.lotStore (shared): the feature's statements; "…" marks code left out
  readonly lotStore = new LotStore(this.changes);

// src/world/doc.ts:299-303 constructor RoadDoc.constructor: the feature's own
  constructor() {
    // A building or a lot made in a loaded tile takes its id from the tile's range, as every other record (`tileIdAt`).
    this.buildings.idAt = (at) => this.tileIdAt('building', at);
    this.lotStore.idAt = (at) => this.tileIdAt('lot', at);
  }

// src/world/doc.ts:305-307 slice of src/world/doc.ts#RoadDoc.lots (shared): the feature's statements; "…" marks code left out
  get lots(): readonly Lot[] {
    return this.lotStore.list();
  }

// src/world/doc.ts:313-315 getter RoadDoc.nextLotId: the feature's own
  get nextLotId(): number {
    return this.lotStore.nextId;
  }

// src/world/doc.ts:317-319 method RoadDoc.lot: the feature's own
  lot(id: number): Lot | undefined {
    return this.lotStore.get(id);
  }

// src/world/doc.ts:321-323 slice of src/world/doc.ts#RoadDoc.lotsIn (shared): the feature's statements; "…" marks code left out
  lotsIn(rect: ChangeRect): Lot[] {
    return this.lotStore.in(rect);
  }

// src/world/doc.ts:325-327 method RoadDoc.lotAt: the feature's own
  lotAt(p: Vec2): Lot | undefined {
    return this.lotStore.at(p);
  }

// src/world/doc.ts:329-329 slice of src/world/doc.ts#RoadDoc.openLotsIndex (shared): the feature's statements; "…" marks code left out
  private openLotsIndex: OpenLots | null = null;

// src/world/doc.ts:330-338 slice of src/world/doc.ts#RoadDoc.openLots (shared): the feature's statements; "…" marks code left out
  get openLots(): OpenLots {
    return this.openLotsIndex ??= new OpenLots({
      lots: this.lotStore,
      standing: (id) => this.buildings.has(id as BuildingId),
      place: (id) => { const b = this.buildings.get(id as BuildingId); return b ? { x: b.x, y: b.y } : null; },
      buildingsRevision: () => this.buildings.revision,
      buildingsSince: (serial) => this.changes.since(serial, ['buildings']),
    });
  }

// src/world/doc.ts:1464-1464 slice of src/world/doc.ts#RoadDoc.tileGrown (shared): the feature's statements; "…" marks code left out
  private tileGrown = new Map<number, { readonly grown: PreparedGrown; readonly lots: ReadonlyMap<number, Lot> }>();

// src/world/doc.ts:1492-1499 method RoadDoc.tileIdAt: the feature's own
  tileIdAt(kind: 'building' | 'lot', at: { readonly x: number; readonly y: number }): number | null {
    const key = OWNER.at(at);
    const next = this.tileContents.has(key) ? this.tileNext.get(key) : undefined;
    if (!next) return null;
    const id = next[kind]++;
    if (id >= tileIdBase(key) + TILE_COUNTED) throw new Error(`tile ${key} has no ${kind} id left`);
    return id;
  }

// src/world/doc.ts:1853-1899 slice of src/world/doc.ts#RoadDoc.layOver (shared): the feature's statements; "…" marks code left out
  private layOver(copy: RoadDoc): void {
    // … 39 lines
    copy.buildings.forkFrom(this.buildings);
    copy.lotStore.forkFrom(this.lotStore);
    copy.openLotsIndex = null;
  // … 3 lines
  }

// src/world/doc.ts:1902-1950 slice of src/world/doc.ts#RoadDoc.takeFork (shared): the feature's statements; "…" marks code left out
  private takeFork(copy: RoadDoc): void {
    // … 11 lines
    this.buildings.takeFork(copy.buildings);
    this.lotStore.takeFork(copy.lotStore);
  // … 34 lines
  }

// src/world/doc.ts:1969-1978 slice of src/world/doc.ts#RoadDoc.ownList (shared): the feature's statements; "…" marks code left out
  private ownList(part: DocListPart): void {
    // … 5 lines
    forkWork.copied += list.length;
  // … 2 lines
  }

// src/world/doc.ts:2114-2118 slice of src/world/doc.ts#RoadDoc.buildingsMovedIn (shared): the feature's statements; "…" marks code left out
  private buildingsMovedIn(source: RoadDoc): readonly BuildingId[] | null {
    // … 2 lines
    return source.buildings.changedSince(fork.version);
  }

// src/world/doc.ts:2121-2125 slice of src/world/doc.ts#RoadDoc.lotsMovedIn (shared): the feature's statements; "…" marks code left out
  private lotsMovedIn(source: RoadDoc): readonly number[] | null {
    // … 2 lines
    return source.lotStore.changedSince(fork.version);
  }

// src/world/doc.ts:2200-2212 slice of src/world/doc.ts#RoadDoc.listen (shared): the feature's statements; "…" marks code left out
  private listen(rec: EditRecorder | null): void {
    // … 9 lines
    this.buildings.watch(rec ? (id, was) => this.note('buildings', id, was) : null);
    this.lotStore.watch(rec ? (id, was) => this.note('lots', id, was) : null, rec ? () => this.noteList('lotKeys') : null);
  }

// src/world/doc.ts:2201-2203 slice of src/world/doc.ts#RoadDoc.listen.tell (shared): the feature's statements; "…" marks code left out
    const tell = <K extends number, V>(map: Map<K, V>, part: RecordPart): void => {
      if (map instanceof RecordMap) map.before = rec ? (key: K, was: V | undefined) => this.note(part, key, was) : null;
    };

// src/world/doc.ts:2215-2226 slice of src/world/doc.ts#RoadDoc.recordOf (shared): the feature's statements; "…" marks code left out
  private recordOf(part: RecordPart, id: number): unknown {
    switch (part) {
      // … 6 lines
      case 'buildings': return this.buildings.get(id as BuildingId);
      case 'lots': return this.lotStore.get(id);
    }
  }

// src/world/doc.ts:2229-2231 slice of src/world/doc.ts#RoadDoc.allocatorMarks (shared): the feature's statements; "…" marks code left out
  allocatorMarks(): number[] {
    return [this.nodeIds.peek, this.segIds.peek, this.poleIds.peek, this.spanIds.peek, this.barrierIds.peek, this.landscapeIds.peek, this.nextTerrainId, this.buildings.nextId];
  }

// src/world/doc.ts:2233-2337 slice of src/world/doc.ts#RoadDoc.restore (shared): the feature's statements; "…" marks code left out
  restore(r: Restoration): void {
    // … 10 lines
    const lotRecords: { id: number; value: Lot | undefined }[] = [];
    // … 26 lines
    for (const { part, id, value } of r.records) {
      // … 1 lines
      switch (part) {
        // … 14 lines
        case 'buildings': {
          // … 1 lines
          const record = value instanceof MemberDiff ? value.over(this.buildings.get(id as BuildingId) as Record<string, unknown> | undefined) : value;
          this.buildings.restore(id as BuildingId, record as Building | undefined);
        // … 1 lines
        }
        // … 1 lines
        case 'lots': lotRecords.push({ id, value: value as Lot | undefined }); break;
      }
    }
    if (lotRecords.length) this.lotStore.restore(lotRecords, 'restaurados');
    for (const { part, start, remove, items } of r.lists) {
      // … 4 lines
      const gone = part === 'lotKeys' ? this.lotStore.spliceKeys(start, remove, put as string[]) : spliceIn(this.listOf(part) as unknown[], start, remove, put);
      if (part === 'elements') {
        for (const e of gone as ElementItem[]) this.elementGrid?.remove(e);
      // … 1 lines
      } else if (part === 'trees') {
        for (const t of gone as PlantedTree[]) this.treeGrid?.remove(t);
      // … 1 lines
      }
      // … 3 lines
      const left = listBox(part, gone), came = listBox(part, items);
      if (left === undefined || came === undefined) { listRects.set(kind, null); continue; }
      if (left) rects.push(left);
      if (came) rects.push(came);
    // … 1 lines
    }
  // … 16 lines
  }

// src/world/doc.ts:2340-2355 slice of src/world/doc.ts#RoadDoc.listOf (shared): the feature's statements; "…" marks code left out
  listOf(part: ListPart): readonly unknown[] {
    switch (part) {
      // … 10 lines
      case 'lotKeys': return this.lotStore.keys;
    // … 1 lines
    }
  }

// src/world/doc.ts:2358-2372 slice of src/world/doc.ts#RoadDoc.valueOf (shared): the feature's statements; "…" marks code left out
  valueOf(part: ValuePart): unknown {
    switch (part) {
      // … 9 lines
      case 'nextLotId': return this.lotStore.nextId;
    // … 1 lines
    }
  }

// src/world/doc.ts:2374-2400 slice of src/world/doc.ts#RoadDoc.restoreValue (shared): the feature's statements; "…" marks code left out
  private restoreValue(part: ValuePart, raw: unknown, kinds: Set<DocChangeKind>): void {
    // … 1 lines
    switch (part) {
      // … 9 lines
      case 'nextLotId': this.lotStore.restoreNextId(value as number); return;
      case 'allocators': {
        // … 8 lines
        this.buildings.restoreNextId(buildings!);
      // … 1 lines
      }
    }
  }

// src/world/doc.ts:2404-2552 slice of src/world/doc.ts#RoadDoc.replaceContents (shared): the feature's statements; "…" marks code left out
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
  // … 1 lines
  } {
    // … 33 lines
    if (whole) this.buildings.takeWhole(source.buildings);
    else this.buildings.replaceWith(source.buildings, buildingIds);
    // … 2 lines
    if (moved('zones') && (whole || !sameText(this.zones, source.zones) || !sameText(this.zoneMarks, source.zoneMarks))) {
      // … 3 lines
      pushAll(this.zones, source.zones.map((zone) => ({ ...zone, buildingIds: [...zone.buildingIds] })));
    // … 3 lines
    }
    // … 1 lines
    if (whole) this.lotStore.takeWhole(source.lotStore, 'map replaced whole');
    else if (moved('lots', 'lotKeys')) this.lotStore.replaceWith(source.lotStore, lotIds, 'replaced by an edit or an undo');
  // … 96 lines
  }

// src/world/doc.ts:2566-2568 slice of src/world/doc.ts#RoadDoc.image (shared): the feature's statements; "…" marks code left out
  image(): DocImage {
    return imageOf(this.serialized(false), this.buildings.sections());
  }

// src/world/doc.ts:2581-2583 slice of src/world/doc.ts#RoadDoc.fromBinary (shared): the feature's statements; "…" marks code left out
  static fromBinary(bytes: ArrayBufferLike): RoadDoc {
    return RoadDoc.fromImage(unpackImage(bytes, (b) => { const out = allocate(b.byteLength); new Uint8Array(out).set(b); return out; }));
  }

// src/world/doc.ts:2586-2596 slice of src/world/doc.ts#RoadDoc.toText (shared): the feature's statements; "…" marks code left out
  toText(): string {
    if (this.buildings.size === 0) return JSON.stringify(this.serialized(false));
    // … 7 lines
    return `${JSON.stringify(head).slice(0, -1)},"buildings":${this.buildings.toText()}${after === '{}' ? '' : `,${after.slice(1, -1)}`}}`;
  }

// src/world/doc.ts:2599-2616 slice of src/world/doc.ts#RoadDoc.savedGlobals (shared): the feature's statements; "…" marks code left out
  savedGlobals(part: GlobalPart): Partial<SavedGlobals> {
    switch (part) {
      // … 11 lines
      case 'lotKeys': return this.lotStore.keys.length > 0 ? { lotKeys: [...this.lotStore.keys] } : {};
    // … 2 lines
    }
  }

// src/world/doc.ts:2626-2633 slice of src/world/doc.ts#RoadDoc.reserveAllocators (shared): the feature's statements; "…" marks code left out
  private reserveAllocators(marks: readonly number[]): void {
    // … 5 lines
    if (Number.isInteger(buildings) && buildings! > this.buildings.nextId) this.buildings.restoreNextId(buildings!);
  }

// src/world/doc.ts:2636-2670 slice of src/world/doc.ts#RoadDoc.serialized (shared): the feature's statements; "…" marks code left out
  private serialized(withBuildings: boolean | 'apart', withRecords = true): SerializedDoc {
    return {
      // … 22 lines
      ...(withBuildings && this.buildings.size > 0 ? { buildings: withBuildings === 'apart' ? [] : this.buildings.toJSON() } : {}),
      // … 1 lines
      ...(withRecords && (this.lotStore.size > 0 || this.lotStore.keys.length > 0) ? {
        lots: this.lotStore.toJSON(),
        lotKeys: [...this.lotStore.keys],
      } : {}),
    // … 3 lines
    };
  }

// src/world/doc.ts:2673-2698 slice of src/world/doc.ts#RoadDoc.worldPart (shared): the feature's statements; "…" marks code left out
  worldPart(part: WorldPart): Partial<SerializedDoc> {
    switch (part) {
      // … 11 lines
      case 'zones': return {
        ...(this.zones.length > 0 ? { zones: this.zones.map((zone) => ({ ...zone, buildingIds: [...zone.buildingIds] })) } : {}),
      // … 1 lines
      };
    // … 7 lines
    }
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 2 lines
    const repair = options.repair ?? true;
    // … 3 lines
    for (const n of data.nodes) {
      // … 4 lines
      const existing = repair ? nodeAt.get(key) : undefined;
      if (existing) {
        canonicalNode.set(n.id, existing.id);
        if (existing.control === 'auto' && n.control && n.control !== 'auto') {
          existing.control = n.control;
        }
        for (const movement of n.blockedMovements ?? []) {
          if (!existing.blockedMovements.includes(movement)) existing.blockedMovements.push(movement);
        }
        continue;
      }
      // … 2 lines
      const node = nodeRecord(n, repair ? onWorld(n) : n);
      doc.nodes.set(id, node);
      nodeAt.set(key, node);
    // … 1 lines
    }
    // … 81 lines
    for (const p of data.poles ?? []) {
      // … 1 lines
      const at = repair ? onWorld(p) : p;
      doc.putPoleRecord({ id, x: at.x, y: at.y, lamp: p.lamp ?? false });
    // … 1 lines
    }
    // … 15 lines
    for (const raw of data.landscape ?? []) {
      const item = landscapeRecord(raw, repair);
      if (!item) continue;
      doc.putLandscapeRecord(item);
      doc.landscapeIds.reserve(item.id);
    }
    // … 1 lines
    for (const raw of data.barriers ?? []) {
      const barrier = barrierRecord(raw, repair);
      if (!barrier) continue;
      doc.putBarrierRecord(barrier);
      doc.barrierIds.reserve(barrier.id);
    }
    // … 1 lines
    if (options.shareBuildings) doc.buildings.shareFrom(options.shareBuildings);
    else if (options.buildingSections) doc.buildings.loadSections(options.buildingSections);
    else if (data.buildings && options.preparedBuildings) doc.buildings.loadPrepared(data.buildings as unknown as readonly Building[]);
    else if (data.buildings) doc.buildings.load(data.buildings);
    // … 2 lines
    for (const zone of data.zones ?? []) {
      if (!Number.isInteger(zone.id) || zone.id < 1 || !isZoneUse(zone.use) || !isZoneDensity(zone.density)) continue;
      if (![zone.x0, zone.y0, zone.x1, zone.y1, zone.seed].every(Number.isFinite)) continue;
      if (zone.x0 >= zone.x1 || zone.y0 >= zone.y1 || !Array.isArray(zone.buildingIds) || zoneIds.has(zone.id)) continue;
      zoneIds.add(zone.id);
      doc.zones.push({ ...zone, buildingIds: zone.buildingIds.filter((id) => Number.isInteger(id) && doc.buildings.has(id as Parameters<typeof doc.buildings.has>[0])) });
      doc.nextZoneId = Math.max(doc.nextZoneId, zone.id + 1);
    }
    for (const mark of data.zoneMarks ?? []) {
      if (!isZoneMark(mark)) continue;
      const standing = mark.building !== undefined && doc.buildings.has(mark.building as Parameters<typeof doc.buildings.has>[0]);
      const { building: _gone, ...free } = mark;
      doc.zoneMarks.push(standing ? { ...mark } : free);
    }
    // … 1 lines
    const lots: Lot[] = [];
    for (const lot of data.lots ?? []) {
      if (!isLot(lot) || lotIds.has(lot.id)) continue;
      lotIds.add(lot.id);
      lots.push(lotRecord(lot, (id) => doc.buildings.has(id as BuildingId)));
    }
    doc.lotStore.load(lots, (data.lotKeys ?? []).filter((key): key is string => typeof key === 'string'));
  // … 12 lines
  }

// src/world/doc.ts:3095-3099 slice of src/world/doc.ts#lotRecord (shared): the feature's statements; "…" marks code left out
function lotRecord(lot: Lot, standing: (id: number) => boolean): Lot {
  const { building: _gone, ...free } = { ...lot, corners: lot.corners.map((q) => ({ x: q.x, y: q.y })),
    ...(lot.use !== undefined ? { use: zoneUseName(lot.use) } : {}), ...(lot.density !== undefined ? { density: zoneDensityName(lot.density) } : {}) };
  return lot.building !== undefined && standing(lot.building) ? { ...free, building: lot.building } : free;
}

// src/world/doc.ts:3111-3114 slice of src/world/doc.ts#PreparedGrown (shared): the feature's statements; "…" marks code left out
export interface PreparedGrown {
  readonly rows: GrownRows;
// … 1 lines
}

// src/world/doc.ts:3116-3141 slice of src/world/doc.ts#TileRecords (shared): the feature's statements; "…" marks code left out
export interface TileRecords {
  // … 14 lines
  readonly grown?: GrownRows;
  readonly lots: readonly Lot[];
// … 8 lines
}

// src/world/doc.ts:3181-3255 slice of src/world/doc.ts#SerializedDoc (shared): the feature's statements; "…" marks code left out
export interface SerializedDoc {
  // … 60 lines
  readonly buildings?: readonly SerializedBuilding[];
  // … 1 lines
  readonly zones?: readonly Zone[];
  readonly zoneMarks?: readonly ZoneMark[];
  // … 1 lines
  readonly lots?: readonly Lot[];
// … 7 lines
}
