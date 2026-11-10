// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:177-177 slice of src/world/doc.ts#RoadDoc.landscapeMap (shared): the feature's statements; "…" marks code left out
  private landscapeMap: Map<number, LandscapeItem> = new RecordMap();

// src/world/doc.ts:178-178 slice of src/world/doc.ts#RoadDoc.landscape (shared): the feature's statements; "…" marks code left out
  get landscape(): ReadonlyMap<number, LandscapeItem> { return this.landscapeMap; }

// src/world/doc.ts:200-200 slice of src/world/doc.ts#RoadDoc.terrainStamps (shared): the feature's statements; "…" marks code left out
  readonly terrainStamps: TerrainStamp[] = [];

// src/world/doc.ts:202-202 slice of src/world/doc.ts#RoadDoc.terrainRelief (shared): the feature's statements; "…" marks code left out
  terrainRelief: ReliefVersion = RELIEF_LEGACY;

// src/world/doc.ts:206-206 slice of src/world/doc.ts#RoadDoc.nature (shared): the feature's statements; "…" marks code left out
  nature: NatureSettings | null = null;

// src/world/doc.ts:208-208 slice of src/world/doc.ts#RoadDoc.terrainPaint (shared): the feature's statements; "…" marks code left out
  readonly terrainPaint: PaintDab[] = [];

// src/world/doc.ts:210-210 slice of src/world/doc.ts#RoadDoc.fogDabs (shared): the feature's statements; "…" marks code left out
  readonly fogDabs: FogDab[] = [];

// src/world/doc.ts:211-211 slice of src/world/doc.ts#RoadDoc.fogSettings (shared): the feature's statements; "…" marks code left out
  fogSettings: FogSettings = DEFAULT_FOG;

// src/world/doc.ts:213-213 slice of src/world/doc.ts#RoadDoc.gullyDabs (shared): the feature's statements; "…" marks code left out
  readonly gullyDabs: GullyDab[] = [];

// src/world/doc.ts:214-214 slice of src/world/doc.ts#RoadDoc.gullyAuto (shared): the feature's statements; "…" marks code left out
  gullyAuto = DEFAULT_GULLY_AUTO;

// src/world/doc.ts:216-216 slice of src/world/doc.ts#RoadDoc.clouds (shared): the feature's statements; "…" marks code left out
  readonly clouds: PlacedCloud[] = [];

// src/world/doc.ts:218-218 slice of src/world/doc.ts#RoadDoc.weather (shared): the feature's statements; "…" marks code left out
  weather: Weather = DEFAULT_WEATHER;

// src/world/doc.ts:222-222 slice of src/world/doc.ts#RoadDoc.trees (shared): the feature's statements; "…" marks code left out
  readonly trees: PlantedTree[] = [];

// src/world/doc.ts:224-224 slice of src/world/doc.ts#RoadDoc.treeGrid (shared): the feature's statements; "…" marks code left out
  private treeGrid: ItemGrid<PlantedTree> | null = null;

// src/world/doc.ts:226-232 slice of src/world/doc.ts#RoadDoc.treeCells (shared): the feature's statements; "…" marks code left out
  private get treeCells(): ItemGrid<PlantedTree> {
    if (!this.treeGrid) {
      // … 1 lines
      for (const t of this.trees) this.treeGrid.add(t);
    }
  // … 1 lines
  }

// src/world/doc.ts:235-239 slice of src/world/doc.ts#RoadDoc.treesIn (shared): the feature's statements; "…" marks code left out
  treesIn(rect: ChangeRect): PlantedTree[] {
    // … 2 lines
    return this.treeCells.near(rect);
  }

// src/world/doc.ts:241-241 slice of src/world/doc.ts#RoadDoc.treeClearings (shared): the feature's statements; "…" marks code left out
  readonly treeClearings: TreeClearing[] = [];

// src/world/doc.ts:576-578 slice of src/world/doc.ts#RoadDoc.addLandscape (shared): the feature's statements; "…" marks code left out
  addLandscape(kind: LandscapeKind, at: { x: number; y: number }, extra: { signType?: SignType; text?: string; planted?: number } = {}): LandscapeItem {
    return this.addLandscapeMany([{ kind, at, extra }])[0]!;
  }

// src/world/doc.ts:581-594 slice of src/world/doc.ts#RoadDoc.addLandscapeMany (shared): the feature's statements; "…" marks code left out
  addLandscapeMany(list: readonly { readonly kind: LandscapeKind; readonly at: { x: number; y: number };
    readonly extra?: { signType?: SignType; text?: string; planted?: number } }[]): LandscapeItem[] {
    const items = list.map(({ kind, at, extra = {} }) => {
      const on = onWorld(at);
      const text = extra.text?.slice(0, SIGN_TEXT_MAX);
      const item: LandscapeItem = { id: this.takeId('landscape', on, this.landscapeIds), kind, x: on.x, y: on.y,
        ...(extra.signType ? { signType: extra.signType } : {}), ...(text ? { text } : {}),
        ...(extra.planted !== undefined && Number.isFinite(extra.planted) ? { planted: extra.planted } : {}) };
      this.putLandscapeRecord(item);
      return item;
    });
    if (items.length) this.changes.record('landscape', items.map((item) => rectAround([item], ITEM_PAD)!), { ids: items.map((item) => item.id) });
    return items;
  }

// src/world/doc.ts:596-602 slice of src/world/doc.ts#RoadDoc.removeLandscape (shared): the feature's statements; "…" marks code left out
  removeLandscape(id: number): boolean {
    const item = this.landscape.get(id);
    if (!item) return false;
    // … 1 lines
    this.changes.record('landscape', [rectAround([item], ITEM_PAD)!], { ids: [id] });
  // … 1 lines
  }

// src/world/doc.ts:605-607 slice of src/world/doc.ts#RoadDoc.landscapeIn (shared): the feature's statements; "…" marks code left out
  landscapeIn(rect: ChangeRect): LandscapeItem[] {
  // … 1 lines
  }

// src/world/doc.ts:610-612 slice of src/world/doc.ts#RoadDoc.landscapeInAny (shared): the feature's statements; "…" marks code left out
  landscapeInAny(rects: readonly ChangeRect[]): LandscapeItem[] {
  // … 1 lines
  }

// src/world/doc.ts:615-615 slice of src/world/doc.ts#RoadDoc.landscapeAt (shared): the feature's statements; "…" marks code left out
  private readonly landscapeAt = (id: number): LandscapeItem => this.landscapeMap.get(id)!;

// src/world/doc.ts:618-625 slice of src/world/doc.ts#RoadDoc.putLandscapeRecord (shared): the feature's statements; "…" marks code left out
  private putLandscapeRecord(item: LandscapeItem): void {
    // … 1 lines
    const was = this.landscapeMap.get(item.id);
    if (was) { this.countStreetName(was, -1); this.landscapeCells.remove(was.id, was.x, was.y); }
    this.landscapeMap.set(item.id, item);
    this.landscapeCells.file(item.id, item.x, item.y);
    this.countStreetName(item, 1);
  }

// src/world/doc.ts:627-634 slice of src/world/doc.ts#RoadDoc.deleteLandscapeRecord (shared): the feature's statements; "…" marks code left out
  private deleteLandscapeRecord(id: number): void {
    // … 4 lines
    this.landscapeCells.remove(id, was.x, was.y);
  // … 1 lines
  }

// src/world/doc.ts:636-640 slice of src/world/doc.ts#RoadDoc.countStreetName (shared): the feature's statements; "…" marks code left out
  private countStreetName(item: LandscapeItem, by: 1 | -1): void {
    if (item.kind !== 'streetname' || !item.text) return;
    const n = (this.streetNameCount.get(item.text) ?? 0) + by;
    if (n > 0) this.streetNameCount.set(item.text, n); else this.streetNameCount.delete(item.text);
  }

// src/world/doc.ts:1067-1070 slice of src/world/doc.ts#RoadDoc.setNature (shared): the feature's statements; "…" marks code left out
  setNature(next: NatureSettings | null): void {
    if (JSON.stringify(this.nature) === JSON.stringify(next)) return;
    this.nature = next ? { ...next } : null;    this.changes.record('nature', null);
  }

// src/world/doc.ts:1087-1098 slice of src/world/doc.ts#RoadDoc.withRoom (shared): the feature's statements; "…" marks code left out
  private withRoom<T extends { readonly x: number; readonly y: number }>(part: CappedPart, items: readonly T[]): { readonly taken: T[]; readonly counts: Map<number, number> } {
    // … 1 lines
    const cap = TILE_CAPS[part];
    // … 1 lines
    for (const item of items) {
      // … 1 lines
      if (n >= cap) continue;
    // … 2 lines
    }
  // … 1 lines
  }

// src/world/doc.ts:1107-1115 slice of src/world/doc.ts#RoadDoc.addPaintDab (shared): the feature's statements; "…" marks code left out
  addPaintDab(dab: PaintDab): boolean {
    const { taken, counts } = this.withRoom('terrainPaint', [dab]);
    if (!taken.length) return false;
    // … 1 lines
    this.terrainPaint.push({ ...dab });
    this.keepCounts('terrainPaint', counts);
    this.changes.record('paint', [circleRect(dab)]);
  // … 1 lines
  }

// src/world/doc.ts:1118-1126 method RoadDoc.addFogDab: the feature's own
  addFogDab(dab: FogDab): boolean {
    const { taken, counts } = this.withRoom('fogDabs', [dab]);
    if (!taken.length) return false;
    this.ownList('fogDabs');
    this.fogDabs.push({ ...dab });
    this.keepCounts('fogDabs', counts);
    this.changes.record('fog', [circleRect(dab)]);
    return true;
  }

// src/world/doc.ts:1129-1137 method RoadDoc.addGullyDab: the feature's own
  addGullyDab(dab: GullyDab): boolean {
    const { taken, counts } = this.withRoom('gullyDabs', [dab]);
    if (!taken.length) return false;
    this.ownList('gullyDabs');
    this.gullyDabs.push({ ...dab });
    this.keepCounts('gullyDabs', counts);
    this.changes.record('gullies', [circleRect(dab)]);
    return true;
  }

// src/world/doc.ts:1148-1152 slice of src/world/doc.ts#RoadDoc.setGullyAuto (shared): the feature's statements; "…" marks code left out
  setGullyAuto(amount: number): void {
    const next = Math.min(1, Math.max(0, Number.isFinite(amount) ? amount : DEFAULT_GULLY_AUTO));
    if (next === this.gullyAuto) return;
    this.gullyAuto = next;    this.changes.record('gullies', null);
  }

// src/world/doc.ts:1161-1165 slice of src/world/doc.ts#RoadDoc.setWeather (shared): the feature's statements; "…" marks code left out
  setWeather(change: Partial<Weather>): void {
    const next = readWeather({ ...this.weather, ...change }, this.weather);
    if (JSON.stringify(next) === JSON.stringify(this.weather)) return;
    this.weather = next;    this.changes.record('weather', null);
  }

// src/world/doc.ts:1168-1177 slice of src/world/doc.ts#RoadDoc.plantTrees (shared): the feature's statements; "…" marks code left out
  plantTrees(trees: readonly PlantedTree[]): number {
    if (trees.length === 0) return 0;
    const { taken, counts } = this.withRoom('trees', trees);
    if (!taken.length) return 0;
    // … 1 lines
    for (const t of taken) { const tree = { ...t }; this.trees.push(tree); this.treeGrid?.add(tree); }
    this.keepCounts('trees', counts);
    this.changes.record('trees', [rectAround(taken, ITEM_PAD)!]);
    return taken.length;
  }

// src/world/doc.ts:1180-1195 method RoadDoc.cutTrees: the feature's own
  cutTrees(x: number, y: number, radius: number): boolean {
    const { taken, counts } = this.withRoom('treeClearings', [{ x, y }]);
    if (!taken.length) return false;
    this.ownList('trees');
    this.ownList('treeClearings');
    const before = this.trees.length;
    // DO NOT splice each one out (trees x trees cut), nor walk every tree of the map for each dab of the brush.
    const there = new Set(this.treeCells.near([x - radius, y - radius, x + radius, y + radius]).filter((t) => Math.hypot(t.x - x, t.y - y) <= radius));
    if (there.size) keepWhere(this.trees, (t) => !there.has(t));
    for (const t of there) this.treeCells.remove(t);
    if (this.trees.length !== before) this.changes.record('trees', [circleRect({ x, y, radius })], { detail: `${before - this.trees.length} cut` });
    this.treeClearings.push({ x, y, radius });
    this.keepCounts('treeClearings', counts);
    this.changes.record('clearings', [circleRect({ x, y, radius })]);
    return true;
  }

// src/world/doc.ts:1215-1224 method RoadDoc.addElements: the feature's own
  addElements(items: readonly ElementItem[]): number {
    if (items.length === 0) return 0;
    const { taken, counts } = this.withRoom('elements', items);
    if (!taken.length) return 0;
    this.ownList('elements');
    for (const item of taken) { this.elements.push(item); this.elementGrid?.add(item); }
    this.keepCounts('elements', counts);
    this.changes.record('elements', [elementsBox(taken)!]);
    return taken.length;
  }

// src/world/doc.ts:1227-1239 method RoadDoc.removeElements: the feature's own
  removeElements(x: number, y: number, radius: number, kind: ElementKind | null): number {
    this.ownList('elements');
    // The ones there, by place; the list then kept in one pass without them.
    const there = new Set(this.elementCells.near([x - radius, y - radius, x + radius, y + radius])
      .filter((e) => (kind === null || e.kind === kind) && Math.hypot(e.x - x, e.y - y) <= radius));
    const before = this.elements.length;
    if (there.size) keepWhere(this.elements, (e) => !there.has(e));
    for (const e of there) this.elementCells.remove(e);
    const gone = before - this.elements.length;
    if (gone > 0) {      this.changes.record('elements', [circleRect({ x, y, radius })]);
    }
    return gone;
  }

// src/world/doc.ts:1263-1270 method RoadDoc.addCloud: the feature's own
  addCloud(value: Omit<PlacedCloud, 'id'>): PlacedCloud | null {
    if (this.clouds.length >= MAX_PLACED_CLOUDS) return null;
    this.ownList('clouds');
    const id = this.clouds.reduce((m, c) => Math.max(m, c.id), 0) + 1;
    const cloud = placedCloud(value, id);
    this.clouds.push(cloud);    this.changes.record('clouds', [rectAround([cloud], cloud.size)!], { ids: [id] });
    return cloud;
  }

// src/world/doc.ts:1273-1286 slice of src/world/doc.ts#RoadDoc.addClouds (shared): the feature's statements; "…" marks code left out
  addClouds(values: readonly Omit<PlacedCloud, 'id'>[]): number {
    // … 1 lines
    let id = this.clouds.reduce((m, c) => Math.max(m, c.id), 0);
    // … 1 lines
    for (const value of values) {
      if (this.clouds.length >= MAX_PLACED_CLOUDS) break;
      this.clouds.push(placedCloud(value, ++id));
      added++;
    }
    if (added > 0) {      const placed = this.clouds.slice(-added);
      this.changes.record('clouds', [rectAround(placed, Math.max(...placed.map((c) => c.size)))!]);
    }
  // … 1 lines
  }

// src/world/doc.ts:1288-1295 method RoadDoc.updateCloud: the feature's own
  updateCloud(id: number, change: Partial<Omit<PlacedCloud, 'id'>>): void {
    const i = this.clouds.findIndex((c) => c.id === id);
    if (i < 0) return;
    this.ownList('clouds');
    const was = this.clouds[i]!;
    const now = { ...was, ...change, id };
    this.clouds[i] = now;    this.changes.record('clouds', [rectAround([was, now], Math.max(was.size, now.size))!], { ids: [id] });
  }

// src/world/doc.ts:1297-1302 slice of src/world/doc.ts#RoadDoc.removeCloud (shared): the feature's statements; "…" marks code left out
  removeCloud(id: number): void {
    const i = this.clouds.findIndex((c) => c.id === id);
    if (i < 0) return;
    // … 1 lines
    const [was] = this.clouds.splice(i, 1);    this.changes.record('clouds', [rectAround([was!], was!.size)!], { ids: [id] });
  }

// src/world/doc.ts:1312-1316 slice of src/world/doc.ts#RoadDoc.setFogSettings (shared): the feature's statements; "…" marks code left out
  setFogSettings(next: Partial<FogSettings>): void {
    const merged = readFogSettings({ ...this.fogSettings, ...next });
    if (JSON.stringify(merged) === JSON.stringify(this.fogSettings)) return;
    this.fogSettings = merged;    this.changes.record('fog', null);
  }

// src/world/doc.ts:1327-1336 slice of src/world/doc.ts#RoadDoc.addTerrainStamp (shared): the feature's statements; "…" marks code left out
  addTerrainStamp(value: Omit<TerrainStamp, 'id'>): TerrainStamp | null {
    const { taken, counts } = this.withRoom('terrainStamps', [value]);
    if (!taken.length) return null;
    // … 1 lines
    const stamp: TerrainStamp = { ...value, id: this.nextTerrainId++ };
    this.terrainStamps.push(stamp);
    this.keepCounts('terrainStamps', counts);
    this.changes.record('terrain', [circleRect(stamp)], { ids: [stamp.id] });
    return stamp;
  }

// src/world/doc.ts:1338-1345 slice of src/world/doc.ts#RoadDoc.clearTerrain (shared): the feature's statements; "…" marks code left out
  clearTerrain(): void {
    // … 3 lines
    const ids = this.terrainStamps.map((s) => s.id);
    // … 1 lines
    this.changes.record('terrain', where, { ids });
  }

// src/world/doc.ts:1853-1899 slice of src/world/doc.ts#RoadDoc.layOver (shared): the feature's statements; "…" marks code left out
  private layOver(copy: RoadDoc): void {
    // … 20 lines
    copy.terrainRelief = this.terrainRelief;
    // … 1 lines
    copy.nature = this.nature;
    copy.fogSettings = this.fogSettings;
    copy.gullyAuto = this.gullyAuto;
    copy.weather = this.weather;
  // … 19 lines
  }

// src/world/doc.ts:1902-1950 slice of src/world/doc.ts#RoadDoc.takeFork (shared): the feature's statements; "…" marks code left out
  private takeFork(copy: RoadDoc): void {
    // … 9 lines
    (copy.landscapeMap as ForkMap<number, LandscapeItem>).applyTo(this.landscapeMap, (_, item) => this.putLandscapeRecord(item), (id) => this.deleteLandscapeRecord(id));
    // … 4 lines
    for (const part of DOC_LISTS) {
      // … 4 lines
      if (grid) {
        // … 1 lines
        for (const item of mine) if (!kept.has(item)) grid.remove(item as PlantedTree & ElementItem);
        for (const item of theirs) if (!had.has(item)) grid.add(item as PlantedTree & ElementItem);
      }
    // … 2 lines
    }
    // … 1 lines
    this.terrainRelief = copy.terrainRelief;
    // … 1 lines
    this.nature = copy.nature;
    this.fogSettings = copy.fogSettings;
    this.gullyAuto = copy.gullyAuto;
    this.weather = copy.weather;
  // … 13 lines
  }

// src/world/doc.ts:2073-2104 slice of src/world/doc.ts#RoadDoc.replaceWith (shared): the feature's statements; "…" marks code left out
  replaceWith(source: RoadDoc): void {
    // … 13 lines
    const stamps = !moved('terrain') ? { rects: [], ids: [] }
      : this.terrainRelief !== source.terrainRelief ? null : stampDifferences(this.terrainStamps, source.terrainStamps);
    // … 6 lines
    if (changed.land || (stamps !== null && stamps.rects.length)) {
      this.changes.record('terrain', where(stamps?.rects), stamps?.ids ? { ids: stamps.ids } : {});
    }
  // … 6 lines
  }

// src/world/doc.ts:2233-2337 slice of src/world/doc.ts#RoadDoc.restore (shared): the feature's statements; "…" marks code left out
  restore(r: Restoration): void {
    // … 37 lines
    for (const { part, id, value } of r.records) {
      // … 1 lines
      switch (part) {
        // … 8 lines
        case 'landscape':
          if (value === undefined) this.deleteLandscapeRecord(id); else this.putLandscapeRecord(structuredClone(value) as LandscapeItem);
          kinds.add('landscape'); landscapeIds.push(id); break;
      // … 11 lines
      }
    }
    // … 1 lines
    for (const { part, start, remove, items } of r.lists) {
      // … 5 lines
      if (part === 'elements') {
      // … 2 lines
      } else if (part === 'trees') {
        for (const t of gone as PlantedTree[]) this.treeGrid?.remove(t);
        for (const t of put as PlantedTree[]) this.treeGrid?.add(t);
      }
    // … 8 lines
    }
  // … 16 lines
  }

// src/world/doc.ts:2258-2268 slice of src/world/doc.ts#RoadDoc.restore.itemRects (shared): the feature's statements; "…" marks code left out
    const itemRects = (): void => {
      for (const { part, id } of r.records) {
        // … 5 lines
        if (part === 'landscape') { const item = this.landscape.get(id); if (item) landscape.push(rectAround([item], ITEM_PAD)!); }
      // … 1 lines
      }
    };

// src/world/doc.ts:2358-2372 slice of src/world/doc.ts#RoadDoc.valueOf (shared): the feature's statements; "…" marks code left out
  valueOf(part: ValuePart): unknown {
    switch (part) {
      case 'terrainRelief': return this.terrainRelief;
      // … 1 lines
      case 'nature': return this.nature;
      case 'fogSettings': return this.fogSettings;
      case 'gullyAuto': return this.gullyAuto;
      case 'weather': return this.weather;
    // … 5 lines
    }
  }

// src/world/doc.ts:2374-2400 slice of src/world/doc.ts#RoadDoc.restoreValue (shared): the feature's statements; "…" marks code left out
  private restoreValue(part: ValuePart, raw: unknown, kinds: Set<DocChangeKind>): void {
    // … 1 lines
    switch (part) {
      case 'terrainRelief': this.terrainRelief = value as ReliefVersion; kinds.add('terrain'); return;
      // … 1 lines
      case 'nature': this.nature = value as NatureSettings | null; kinds.add('nature'); return;
      case 'fogSettings': this.fogSettings = value as FogSettings; kinds.add('fog'); return;
      case 'gullyAuto': this.gullyAuto = value as number; kinds.add('gullies'); return;
      case 'weather': this.weather = value as Weather; kinds.add('weather'); return;
    // … 16 lines
    }
  }

// src/world/doc.ts:2404-2552 slice of src/world/doc.ts#RoadDoc.replaceContents (shared): the feature's statements; "…" marks code left out
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
  // … 1 lines
  } {
    // … 3 lines
    const landMoved = moved('terrain') && (whole || !sameStamps(this.terrainStamps, source.terrainStamps) || this.terrainRelief !== source.terrainRelief);
    // … 19 lines
    if (moved('landscape')) {
      // … 1 lines
      for (const item of source.landscape.values()) this.putLandscapeRecord({ ...item });
    }
    // … 33 lines
    if (moved('nature') && (whole || !sameText(this.nature, source.nature))) {
      this.nature = source.nature ? { ...source.nature } : null;
      others.push('nature');
    }
    // … 5 lines
    if (moved('weather') && (whole || !sameText(this.weather, source.weather))) {
      this.weather = { ...source.weather };
      others.push('weather');
    }
    // … 29 lines
    if (moved('gullies') && (whole || this.gullyAuto !== source.gullyAuto || !sameText(this.gullyDabs, source.gullyDabs))) {
      this.ownList('gullyDabs');
      this.gullyDabs.length = 0;
      pushAll(this.gullyDabs, source.gullyDabs.map((dab) => ({ ...dab })));
      this.gullyAuto = source.gullyAuto;
      others.push('gullies');
    }
    // … 1 lines
    if (moved('fog') && (whole || !sameText(this.fogDabs, source.fogDabs) || !sameText(this.fogSettings, source.fogSettings))) {
      this.ownList('fogDabs');
      this.fogDabs.length = 0;
      pushAll(this.fogDabs, source.fogDabs.map((dab) => ({ ...dab })));
      this.fogSettings = { ...source.fogSettings };
      others.push('fog');
    }
    // … 13 lines
    if (landMoved) {
      this.ownList('terrainStamps');
      this.terrainStamps.length = 0;
      pushAll(this.terrainStamps, source.terrainStamps.map((stamp) => ({ ...stamp })));
      this.terrainRelief = source.terrainRelief;
    }
    // … 8 lines
    return { roads: roadsChanged, land: landMoved, barriers: barriersChanged, others };
  }

// src/world/doc.ts:2599-2616 slice of src/world/doc.ts#RoadDoc.savedGlobals (shared): the feature's statements; "…" marks code left out
  savedGlobals(part: GlobalPart): Partial<SavedGlobals> {
    switch (part) {
      case 'values': return {
        // … 2 lines
        ...(this.terrainRelief !== RELIEF_LEGACY ? { relief: this.terrainRelief } : {}),
        // … 1 lines
        ...(this.nature ? { nature: { ...this.nature } } : {}),
        ...(!samePlain(this.weather, DEFAULT_WEATHER) ? { weather: { ...this.weather } } : {}),
        fogSettings: { ...this.fogSettings },
        gullyAuto: this.gullyAuto,
      // … 1 lines
      };
    // … 3 lines
    }
  }

// src/world/doc.ts:2636-2670 slice of src/world/doc.ts#RoadDoc.serialized (shared): the feature's statements; "…" marks code left out
  private serialized(withBuildings: boolean | 'apart', withRecords = true): SerializedDoc {
    return {
      // … 18 lines
      ...(withRecords && this.landscape.size > 0 ? { landscape: inIdOrder(this.landscape.values()).map(savedLandscape) } : {}),
    // … 12 lines
    };
  }

// src/world/doc.ts:2673-2698 slice of src/world/doc.ts#RoadDoc.worldPart (shared): the feature's statements; "…" marks code left out
  worldPart(part: WorldPart): Partial<SerializedDoc> {
    switch (part) {
      // … 1 lines
      case 'terrain': return { terrain: this.terrainStamps.map((stamp) => ({ ...stamp })), ...(this.terrainRelief !== RELIEF_LEGACY ? { relief: this.terrainRelief } : {}) };
      // … 1 lines
      case 'nature': return this.nature ? { nature: { ...this.nature } } : {};
      case 'fog': return this.fogDabs.length > 0 || JSON.stringify(this.fogSettings) !== JSON.stringify(DEFAULT_FOG)
        ? { fog: { dabs: this.fogDabs.map((dab) => ({ ...dab })), settings: { ...this.fogSettings } } } : {};
      case 'gullies': return this.gullyDabs.length > 0 || this.gullyAuto !== DEFAULT_GULLY_AUTO
        ? { gullies: { dabs: this.gullyDabs.map((dab) => ({ ...dab })), auto: this.gullyAuto } } : {};
      // … 1 lines
      case 'weather': return JSON.stringify(this.weather) !== JSON.stringify(DEFAULT_WEATHER) ? { weather: { ...this.weather } } : {};
    // … 12 lines
    }
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 45 lines
    for (const dab of data.paint ?? []) {
      if (!isPaintKind(dab.kind) || ![dab.x, dab.y, dab.radius, dab.strength].every(Number.isFinite)) continue;
    // … 1 lines
    }
    // … 3 lines
    if (data.weather) { doc.weather = readWeather(data.weather); doc.changes.record('weather', null, loaded); }
    // … 1 lines
    for (const raw of data.trees ?? []) {
      const tree = readPlantedTree(raw);
      // … 1 lines
      if (tree) doc.trees.push(tree);
    }
    // … 1 lines
    for (const raw of data.treeClearings ?? []) {
      const clearing = readTreeClearing(raw);
      if (clearing) doc.treeClearings.push(clearing);
    }
    // … 9 lines
    for (const raw of data.clouds ?? []) {
      const cloud = readCloud(raw);
      if (!cloud || doc.clouds.length >= MAX_PLACED_CLOUDS || cloudIds.has(cloud.id)) continue;
      cloudIds.add(cloud.id);
      doc.clouds.push(cloud);
    }
    // … 2 lines
    if (data.gullies) {
      for (const raw of data.gullies.dabs ?? []) {
        const dab = readGullyDab(raw);
        if (dab) doc.gullyDabs.push(dab);
      }
      // … 1 lines
      if (typeof auto === 'number' && Number.isFinite(auto)) doc.gullyAuto = Math.min(1, Math.max(0, auto));
    // … 1 lines
    }
    // … 1 lines
    if (data.fog) {
      for (const raw of data.fog.dabs ?? []) {
        const dab = readFogDab(raw);
        if (dab) doc.fogDabs.push(dab);
      }
      doc.fogSettings = readFogSettings(data.fog.settings);
    // … 1 lines
    }
    // … 1 lines
    doc.terrainRelief = readRelief(data.relief) ?? RELIEF_LEGACY;
    // … 2 lines
    doc.nature = isNatureSettings(data.nature) ? { region: data.nature.region, seed: data.nature.seed } : null;
    for (const stamp of data.terrain ?? []) {
      // … 1 lines
      doc.nextTerrainId = Math.max(doc.nextTerrainId, stamp.id + 1);
    }
    // … 23 lines
    for (const raw of data.landscape ?? []) {
      // … 3 lines
      doc.landscapeIds.reserve(item.id);
    }
  // … 48 lines
  }

// src/world/doc.ts:2951-2954 slice of src/world/doc.ts#TILE_CAPS (shared): the feature's statements; "…" marks code left out
const TILE_CAPS: Readonly<Record<CappedPart, number>> = {
  terrainStamps: MAX_TERRAIN_STAMPS, terrainPaint: MAX_PAINT_DABS, fogDabs: MAX_FOG_DABS, gullyDabs: MAX_GULLY_DABS,
  trees: MAX_PLANTED_TREES, treeClearings: MAX_TREE_CLEARINGS, elements: MAX_ELEMENTS,
};

// src/world/doc.ts:3067-3077 slice of src/world/doc.ts#landscapeRecord (shared): the feature's statements; "…" marks code left out
function landscapeRecord(raw: NonNullable<SerializedDoc['landscape']>[number], repair: boolean): LandscapeItem | null {
  if (!isLandscapeKind(raw?.kind) || !Number.isFinite(raw.x) || !Number.isFinite(raw.y) || !Number.isInteger(raw.id)) return null;
  // … 3 lines
  const item: { -readonly [K in keyof LandscapeItem]: LandscapeItem[K] } = { id: raw.id, kind: LANDSCAPE_KINDS.find((k) => k === raw.kind) ?? raw.kind, x: at.x, y: at.y };
  if (isSignType(raw.signType)) item.signType = raw.signType;
  if (typeof raw.text === 'string' && raw.text) item.text = raw.text.slice(0, SIGN_TEXT_MAX);
  if (Number.isFinite(raw.planted)) item.planted = raw.planted as number;
  return item;
}

// src/world/doc.ts:3181-3255 slice of src/world/doc.ts#SerializedDoc (shared): the feature's statements; "…" marks code left out
export interface SerializedDoc {
  // … 34 lines
  readonly terrain?: readonly TerrainStamp[];
// … 38 lines
}

// src/world/doc.ts:3314-3315 slice of src/world/doc.ts#placedCloud (shared): the feature's statements; "…" marks code left out
const placedCloud = (v: Omit<PlacedCloud, 'id'>, id: number): PlacedCloud =>
  ({ id, x: v.x, y: v.y, height: v.height, size: v.size, density: v.density, yaw: v.yaw });

// src/world/doc.ts:3345-3359 slice of src/world/doc.ts#listBox (shared): the feature's statements; "…" marks code left out
function listBox(part: ListPart, items: readonly unknown[]): ChangeRect | null | undefined {
  // … 1 lines
  switch (part) {
    // … 2 lines
    case 'trees': return rectAround(items as readonly PlantedTree[], ITEM_PAD);
    // … 1 lines
    case 'clouds': {
      // … 1 lines
      for (const c of items as readonly PlacedCloud[]) pad = Math.max(pad, c.size);
      return rectAround(items as readonly PlacedCloud[], pad);
    }
  // … 1 lines
  }
}

// src/world/doc.ts:3422-3436 slice of src/world/doc.ts#stampDifferences (shared): the feature's statements; "…" marks code left out
function stampDifferences(a: readonly TerrainStamp[], b: readonly TerrainStamp[]): { rects: ChangeRect[]; ids: number[] } {
  // … 2 lines
  const mine = new Map(a.map((s) => [s.id, s]));
  const theirs = new Map(b.map((s) => [s.id, s]));
  for (const [id, s] of mine) {
    const t = theirs.get(id);
    if (t && sameText(s, t)) continue;
    ids.push(id);
    rects.push(circleRect(s));
    if (t) rects.push(circleRect(t));
  }
  for (const [id, t] of theirs) if (!mine.has(id)) { ids.push(id); rects.push(circleRect(t)); }
// … 1 lines
}

// src/world/doc.ts:3439-3471 slice of src/world/doc.ts#itemDifferences (shared): the feature's statements; "…" marks code left out
function itemDifferences(a: RoadDoc, b: RoadDoc, moved: MovedKinds): { utilities: ChangeRect[]; landscape: ChangeRect[]; barriers: ChangeRect[] } {
  // … 12 lines
  if (moved('landscape')) {
    for (const [id, p] of a.landscape) {
      const q = b.landscape.get(id);
      if (q && p.kind === q.kind && p.x === q.x && p.y === q.y && p.signType === q.signType && p.text === q.text && p.planted === q.planted) continue;
      landscape.push(at(p));
      if (q) landscape.push(at(q));
    }
    for (const [id, q] of b.landscape) if (!a.landscape.has(id)) landscape.push(at(q));
  }
// … 10 lines
}

// src/world/doc.ts:3530-3540 slice of src/world/doc.ts#sameStamps (shared): the feature's statements; "…" marks code left out
export function sameStamps(a: readonly TerrainStamp[], b: readonly TerrainStamp[]): boolean {
  if (a.length !== b.length) return false;
  for (let i = 0; i < a.length; i++) {
    const p = a[i] as TerrainStamp;
    const q = b[i] as TerrainStamp;
    if (p === q) continue;
    const keys = new Set([...Object.keys(p), ...Object.keys(q)]) as Set<keyof TerrainStamp>;
    for (const key of keys) if (p[key] !== q[key]) return false;
  }
// … 1 lines
}

// src/world/doc.ts:3552-3559 slice of src/world/doc.ts#samePaint (shared): the feature's statements; "…" marks code left out
function samePaint(a: readonly PaintDab[], b: readonly PaintDab[]): boolean {
  if (a.length !== b.length) return false;
  for (let i = 0; i < a.length; i++) {
    const p = a[i]!, q = b[i]!;
    if (p.kind !== q.kind || p.x !== q.x || p.y !== q.y || p.radius !== q.radius || p.strength !== q.strength) return false;
  }
// … 1 lines
}

// src/world/doc.ts:3692-3694 slice of src/world/doc.ts#savedLandscape (shared): the feature's statements; "…" marks code left out
export const savedLandscape = (item: LandscapeItem): NonNullable<SerializedDoc['landscape']>[number] => ({ id: item.id, kind: item.kind, x: item.x, y: item.y,
  ...(item.signType ? { signType: item.signType } : {}), ...(item.text ? { text: item.text } : {}),
  ...(item.planted !== undefined ? { planted: item.planted } : {}) });
