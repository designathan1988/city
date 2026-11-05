// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:161-161 slice of src/world/doc.ts#RoadDoc.poleMap (shared): the feature's statements; "…" marks code left out
  private poleMap: Map<PoleId, UtilityPole> = new RecordMap();

// src/world/doc.ts:162-162 slice of src/world/doc.ts#RoadDoc.spanMap (shared): the feature's statements; "…" marks code left out
  private spanMap: Map<SpanId, UtilitySpan> = new RecordMap();

// src/world/doc.ts:163-163 slice of src/world/doc.ts#RoadDoc.poleIndex (shared): the feature's statements; "…" marks code left out
  private poleIndex = new PoleIndex();

// src/world/doc.ts:164-164 slice of src/world/doc.ts#RoadDoc.poles (shared): the feature's statements; "…" marks code left out
  get poles(): ReadonlyMap<PoleId, UtilityPole> { return this.poleMap; }

// src/world/doc.ts:165-165 slice of src/world/doc.ts#RoadDoc.poleSpans (shared): the feature's statements; "…" marks code left out
  get poleSpans(): ReadonlyMap<SpanId, UtilitySpan> { return this.spanMap; }

// src/world/doc.ts:169-169 slice of src/world/doc.ts#RoadDoc.barrierMap (shared): the feature's statements; "…" marks code left out
  private barrierMap: Map<number, Barrier> = new RecordMap();

// src/world/doc.ts:170-170 slice of src/world/doc.ts#RoadDoc.barriers (shared): the feature's statements; "…" marks code left out
  get barriers(): ReadonlyMap<number, Barrier> { return this.barrierMap; }

// src/world/doc.ts:172-172 slice of src/world/doc.ts#RoadDoc.barrierCells (shared): the feature's statements; "…" marks code left out
  private barrierCells = new CellFiling<number, Barrier>(LANDSCAPE_CELL);

// src/world/doc.ts:436-438 method RoadDoc.pole: the feature's own
  pole(id: PoleId): UtilityPole | undefined {
    return this.poles.get(id);
  }

// src/world/doc.ts:440-447 slice of src/world/doc.ts#RoadDoc.addPole (shared): the feature's statements; "…" marks code left out
  addPole(at: { x: number; y: number }, lamp = false): UtilityPole {
    // … 2 lines
    const pole: UtilityPole = { id, x: on.x, y: on.y, lamp };
    this.putPoleRecord(pole);
    this.changes.record('utilities', [rectAround([pole], ITEM_PAD)!], { ids: [id] });
    return pole;
  }

// src/world/doc.ts:450-452 slice of src/world/doc.ts#RoadDoc.polesIn (shared): the feature's statements; "…" marks code left out
  polesIn(rect: ChangeRect): UtilityPole[] {
    return this.poleIndex.near(rect).map((id) => this.poleMap.get(id)!);
  }

// src/world/doc.ts:455-457 slice of src/world/doc.ts#RoadDoc.spansAt (shared): the feature's statements; "…" marks code left out
  spansAt(id: PoleId): UtilitySpan[] {
    return [...this.poleIndex.spansAt(id)].map((sid) => this.spanMap.get(sid)!);
  }

// src/world/doc.ts:459-463 slice of src/world/doc.ts#RoadDoc.putPoleRecord (shared): the feature's statements; "…" marks code left out
  private putPoleRecord(pole: UtilityPole): void {
    // … 1 lines
    this.poleMap.set(pole.id, pole);
    this.poleIndex.putPole(pole);
  }

// src/world/doc.ts:465-469 slice of src/world/doc.ts#RoadDoc.deletePoleRecord (shared): the feature's statements; "…" marks code left out
  private deletePoleRecord(id: PoleId): void {
    // … 2 lines
    this.poleIndex.deletePole(id);
  }

// src/world/doc.ts:471-479 slice of src/world/doc.ts#RoadDoc.putSpanRecord (shared): the feature's statements; "…" marks code left out
  private putSpanRecord(span: UtilitySpan): void {
    // … 1 lines
    const was = this.spanMap.get(span.id);
    this.spanMap.set(span.id, span);
    // … 1 lines
    if (was && was.a === span.a && was.b === span.b) return;
    if (was) this.poleIndex.deleteSpan(was);
    this.poleIndex.putSpan(span);
  }

// src/world/doc.ts:481-487 slice of src/world/doc.ts#RoadDoc.deleteSpanRecord (shared): the feature's statements; "…" marks code left out
  private deleteSpanRecord(id: SpanId): void {
    // … 4 lines
    this.poleIndex.deleteSpan(was);
  }

// src/world/doc.ts:489-495 slice of src/world/doc.ts#RoadDoc.clearPoleRecords (shared): the feature's statements; "…" marks code left out
  private clearPoleRecords(): void {
    // … 3 lines
    if (this.sharedIndices.delete('poles')) this.poleIndex = new PoleIndex();
    else this.poleIndex.clear();
  }

// src/world/doc.ts:498-509 slice of src/world/doc.ts#RoadDoc.addPoleSpan (shared): the feature's statements; "…" marks code left out
  addPoleSpan(a: PoleId, b: PoleId): UtilitySpan | null {
    // … 1 lines
    if (!this.poles.has(a) || !this.poles.has(b)) return null;
    for (const existing of this.spansAt(a)) {
      if (existing.a === b || existing.b === b) return existing;
    }
    const id = asSpanId(this.takeId('span', this.poles.get(a)!, this.spanIds));
    const span: UtilitySpan = { id, a, b };
    this.putSpanRecord(span);
    this.changes.record('utilities', rectsAlong([this.poles.get(a)!, this.poles.get(b)!], ITEM_PAD), { ids: [a, b, id] });
    return span;
  }

// src/world/doc.ts:512-519 slice of src/world/doc.ts#RoadDoc.addBarrier (shared): the feature's statements; "…" marks code left out
  addBarrier(kind: BarrierKind, points: readonly { x: number; y: number }[]): Barrier | null {
    // … 2 lines
    const barrier: Barrier = { id: this.takeId('barrier', path[0]!, this.barrierIds), kind, points: path.map((p) => ({ x: p.x, y: p.y })) };
    this.putBarrierRecord(barrier);
    this.changes.record('barriers', rectsAlong(barrier.points, ITEM_PAD), { ids: [barrier.id] });
    return barrier;
  }

// src/world/doc.ts:521-527 slice of src/world/doc.ts#RoadDoc.removeBarrier (shared): the feature's statements; "…" marks code left out
  removeBarrier(id: number): boolean {
    const barrier = this.barriers.get(id);
    if (!barrier) return false;
    // … 1 lines
    this.changes.record('barriers', rectsAlong(barrier.points, ITEM_PAD), { ids: [id] });
  // … 1 lines
  }

// src/world/doc.ts:531-533 method RoadDoc.barriersInAny: the feature's own
  barriersInAny(rects: readonly ChangeRect[]): Barrier[] {
    return this.barrierCells.idsMeetingAny(rects).map((id) => this.barrierMap.get(id)!);
  }

// src/world/doc.ts:535-537 slice of src/world/doc.ts#RoadDoc.barriersIn (shared): the feature's statements; "…" marks code left out
  barriersIn(rect: ChangeRect): Barrier[] {
    return this.barrierCells.idsMeeting(rect).map((id) => this.barrierMap.get(id)!);
  }

// src/world/doc.ts:540-544 slice of src/world/doc.ts#RoadDoc.putBarrierRecord (shared): the feature's statements; "…" marks code left out
  private putBarrierRecord(barrier: Barrier): void {
    // … 1 lines
    this.barrierMap.set(barrier.id, barrier);
    this.barrierCells.file(barrier.id, barrier, rectAround(barrier.points)!);
  }

// src/world/doc.ts:546-550 slice of src/world/doc.ts#RoadDoc.deleteBarrierRecord (shared): the feature's statements; "…" marks code left out
  private deleteBarrierRecord(id: number): void {
    // … 2 lines
    this.barrierCells.remove(id);
  }

// src/world/doc.ts:552-556 slice of src/world/doc.ts#RoadDoc.clearBarrierRecords (shared): the feature's statements; "…" marks code left out
  private clearBarrierRecords(): void {
    // … 1 lines
    if (this.sharedIndices.delete('barriers')) this.barrierCells = new CellFiling(LANDSCAPE_CELL);
    else this.barrierCells.copyFrom(new CellFiling(LANDSCAPE_CELL));
  }

// src/world/doc.ts:559-573 slice of src/world/doc.ts#RoadDoc.barrierNear (shared): the feature's statements; "…" marks code left out
  barrierNear(at: { x: number; y: number }, radius: number): Barrier | null {
    let best: Barrier | null = null;
    // … 1 lines
    for (const barrier of this.barriersIn([at.x - radius, at.y - radius, at.x + radius, at.y + radius])) {
      for (let i = 1; i < barrier.points.length; i++) {
        const a = barrier.points[i - 1]!, b = barrier.points[i]!, q = at;
        const dx = b.x - a.x, dy = b.y - a.y;
        const len = dx * dx + dy * dy;
        const t = len > 0 ? Math.max(0, Math.min(1, ((q.x - a.x) * dx + (q.y - a.y) * dy) / len)) : 0;
        const d = Math.hypot(a.x + dx * t - q.x, a.y + dy * t - q.y);
        if (d < bestD) { bestD = d; best = barrier; }
      }
    }
    return best;
  }

// src/world/doc.ts:610-612 method RoadDoc.landscapeInAny: the feature's own
  landscapeInAny(rects: readonly ChangeRect[]): LandscapeItem[] {
    return this.landscapeCells.idsMeetingAny(rects, this.landscapeAt).map((id) => this.landscapeMap.get(id)!);
  }

// src/world/doc.ts:654-668 slice of src/world/doc.ts#RoadDoc.removePole (shared): the feature's statements; "…" marks code left out
  removePole(id: PoleId): void {
    const pole = this.poles.get(id);
    if (!pole) return;
    const reached: { x: number; y: number }[] = [pole];
    // … 2 lines
    for (const span of this.spansAt(id)) {
      const other = this.poles.get(span.a === id ? span.b : span.a);
      if (other) reached.push(other);
      this.deleteSpanRecord(span.id);
      ids.push(span.id);
    }
    // … 1 lines
    this.changes.record('utilities', [rectAround(reached, ITEM_PAD)!], { ids });
  }

// src/world/doc.ts:671-684 slice of src/world/doc.ts#RoadDoc.poleNear (shared): the feature's statements; "…" marks code left out
  poleNear(at: { x: number; y: number }, radius: number): UtilityPole | null {
    let best: UtilityPole | null = null;
    // … 1 lines
    for (const pole of this.polesIn([at.x - radius, at.y - radius, at.x + radius, at.y + radius])) {
      const dx = pole.x - at.x;
      const dy = pole.y - at.y;
      const d = dx * dx + dy * dy;
      if (d <= bestSq) {
        bestSq = d;
        best = pole;
      }
    }
    return best;
  }

// src/world/doc.ts:1007-1014 method RoadDoc.setNodeUturn: the feature's own
  setNodeUturn(id: NodeId, from: SegmentId | undefined): void {
    const was = this.editableNode(id);
    if (!was || was.uturn === from) return;
    const node = this.nodeToWrite(id)!;
    if (from !== undefined) node.uturn = from;
    else delete node.uturn;
    this.markNode(id);
  }

// src/world/doc.ts:1355-1363 method RoadDoc.setNodeCrossing: the feature's own
  setNodeCrossing(id: NodeId, kind: NodeCrossingKind, segment: SegmentId): boolean {
    const was = this.editableNode(id);
    if (!was || was.incident.length !== 2 || !was.incident.includes(segment)) return false;
    const n = this.nodeToWrite(id)!;
    n.crossing = { kind, segment };
    n.control = kind === 'signal' ? 'signal' : 'priority';
    this.markNode(id);
    return true;
  }

// src/world/doc.ts:1853-1899 slice of src/world/doc.ts#RoadDoc.layOver (shared): the feature's statements; "…" marks code left out
  private layOver(copy: RoadDoc): void {
    // … 7 lines
    copy.poleIndex = this.poleIndex;
    // … 1 lines
    copy.barrierCells = this.barrierCells;
  // … 35 lines
  }

// src/world/doc.ts:1902-1950 slice of src/world/doc.ts#RoadDoc.takeFork (shared): the feature's statements; "…" marks code left out
  private takeFork(copy: RoadDoc): void {
    // … 7 lines
    (copy.poleMap as ForkMap<PoleId, UtilityPole>).applyTo(this.poleMap, (_, pole) => this.putPoleRecord(pole), (id) => this.deletePoleRecord(id));
    (copy.spanMap as ForkMap<SpanId, UtilitySpan>).applyTo(this.spanMap, (_, span) => this.putSpanRecord(span), (id) => this.deleteSpanRecord(id));
    // … 1 lines
    (copy.barrierMap as ForkMap<number, Barrier>).applyTo(this.barrierMap, (_, barrier) => this.putBarrierRecord(barrier), (id) => this.deleteBarrierRecord(id));
  // … 36 lines
  }

// src/world/doc.ts:1987-2000 slice of src/world/doc.ts#RoadDoc.ownIndex (shared): the feature's statements; "…" marks code left out
  private ownIndex(kind: IndexKind): void {
    // … 1 lines
    switch (kind) {
      case 'poles':
        this.poleIndex = this.poleIndex.over();
        return;
      case 'barriers':
        this.barrierCells = CellFiling.over(this.barrierCells);
        return;
    // … 3 lines
    }
  }

// src/world/doc.ts:2233-2337 slice of src/world/doc.ts#RoadDoc.restore (shared): the feature's statements; "…" marks code left out
  restore(r: Restoration): void {
    // … 37 lines
    for (const { part, id, value } of r.records) {
      // … 1 lines
      switch (part) {
        // … 6 lines
        case 'poles': if (value === undefined) this.deletePoleRecord(id as PoleId); else this.putPoleRecord(structuredClone(value) as UtilityPole); kinds.add('utilities'); utilityIds.push(id); break;
        case 'poleSpans': if (value === undefined) this.deleteSpanRecord(id as SpanId); else this.putSpanRecord(structuredClone(value) as UtilitySpan); kinds.add('utilities'); utilityIds.push(id); break;
        // … 3 lines
        case 'barriers':
          if (value === undefined) this.deleteBarrierRecord(id); else this.putBarrierRecord(structuredClone(value) as Barrier);
          kinds.add('barriers'); barrierIds.push(id); break;
      // … 8 lines
      }
    }
  // … 39 lines
  }

// src/world/doc.ts:2258-2268 slice of src/world/doc.ts#RoadDoc.restore.itemRects (shared): the feature's statements; "…" marks code left out
    const itemRects = (): void => {
      for (const { part, id } of r.records) {
        if (part === 'poles') { const p = this.poles.get(id as PoleId); if (p) utilities.push(rectAround([p], ITEM_PAD)!); }
        if (part === 'poleSpans') {
          const s = this.poleSpans.get(id as SpanId), p = s && this.poles.get(s.a), q = s && this.poles.get(s.b);
          if (p && q) utilities.push(rectAround([p, q], ITEM_PAD)!);
        }
        // … 1 lines
        if (part === 'barriers') { const b = this.barriers.get(id); if (b) barriers.push(rectAround(b.points, ITEM_PAD)!); }
      }
    };

// src/world/doc.ts:2404-2552 slice of src/world/doc.ts#RoadDoc.replaceContents (shared): the feature's statements; "…" marks code left out
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
  // … 1 lines
  } {
    // … 18 lines
    if (moved('utilities')) {
      // … 1 lines
      for (const pole of source.poles.values()) this.putPoleRecord({ ...pole });
      for (const span of source.poleSpans.values()) this.putSpanRecord({ ...span });
    }
    // … 4 lines
    if (moved('barriers') && (whole || !sameBarriers(this, source))) {
      // … 1 lines
      for (const barrier of source.barriers.values()) this.putBarrierRecord({ ...barrier, points: barrier.points.map((p) => ({ ...p })) });
    // … 1 lines
    }
  // … 113 lines
  }

// src/world/doc.ts:2636-2670 slice of src/world/doc.ts#RoadDoc.serialized (shared): the feature's statements; "…" marks code left out
  private serialized(withBuildings: boolean | 'apart', withRecords = true): SerializedDoc {
    return {
      // … 16 lines
      poles: withRecords ? inIdOrder(this.poles.values()).map(savedPole) : [],
      poleSpans: withRecords ? inIdOrder(this.poleSpans.values()).map(savedSpan) : [],
      // … 2 lines
      ...(withRecords && this.barriers.size > 0 ? { barriers: inIdOrder(this.barriers.values()).map(savedBarrier) } : {}),
    // … 10 lines
    };
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 116 lines
    for (const s of data.poleSpans ?? []) {
      // … 3 lines
      if (!doc.poles.has(a) || !doc.poles.has(b)) continue;
    // … 3 lines
    }
    // … 13 lines
    for (const raw of data.barriers ?? []) {
      // … 3 lines
      doc.barrierIds.reserve(barrier.id);
    }
  // … 41 lines
  }

// src/world/doc.ts:3080-3085 slice of src/world/doc.ts#barrierRecord (shared): the feature's statements; "…" marks code left out
function barrierRecord(raw: NonNullable<SerializedDoc['barriers']>[number], repair: boolean): Barrier | null {
  if (!isBarrierKind(raw.kind) || !Array.isArray(raw.points)) return null;
// … 3 lines
}

// src/world/doc.ts:3439-3471 slice of src/world/doc.ts#itemDifferences (shared): the feature's statements; "…" marks code left out
function itemDifferences(a: RoadDoc, b: RoadDoc, moved: MovedKinds): { utilities: ChangeRect[]; landscape: ChangeRect[]; barriers: ChangeRect[] } {
  // … 2 lines
  if (moved('utilities')) {
    for (const [id, p] of a.poles) { const q = b.poles.get(id); if (!q || p.x !== q.x || p.y !== q.y || p.lamp !== q.lamp) { utilities.push(at(p)); if (q) utilities.push(at(q)); } }
    for (const [id, q] of b.poles) if (!a.poles.has(id)) utilities.push(at(q));
    const spanRect = (doc: RoadDoc, s: UtilitySpan): ChangeRect | null => {
      const p = doc.poles.get(s.a), q = doc.poles.get(s.b);
      return p && q ? rectAround([p, q], ITEM_PAD) : null;
    };
    for (const [id, s] of a.poleSpans) { const t = b.poleSpans.get(id); if (!t || t.a !== s.a || t.b !== s.b) { const r = spanRect(a, s); if (r) utilities.push(r); } }
    for (const [id, t] of b.poleSpans) if (!a.poleSpans.has(id)) { const r = spanRect(b, t); if (r) utilities.push(r); }
  }
  // … 9 lines
  if (moved('barriers')) {
    for (const [id, p] of a.barriers) {
      const q = b.barriers.get(id);
      if (q && sameText(p, q)) continue;
      barriers.push(rectAround(p.points, ITEM_PAD)!);
      if (q) barriers.push(rectAround(q.points, ITEM_PAD)!);
    }
    for (const [id, q] of b.barriers) if (!a.barriers.has(id)) barriers.push(rectAround(q.points, ITEM_PAD)!);
  }
// … 1 lines
}

// src/world/doc.ts:3474-3484 slice of src/world/doc.ts#sameBarriers (shared): the feature's statements; "…" marks code left out
function sameBarriers(a: RoadDoc, b: RoadDoc): boolean {
  if (a.barriers.size !== b.barriers.size) return false;
  for (const [id, p] of a.barriers) {
    const q = b.barriers.get(id);
    if (!q || p.kind !== q.kind || p.points.length !== q.points.length) return false;
    for (let i = 0; i < p.points.length; i++) {
      if (p.points[i]!.x !== q.points[i]!.x || p.points[i]!.y !== q.points[i]!.y) return false;
    }
  }
// … 1 lines
}

// src/world/doc.ts:3690-3690 slice of src/world/doc.ts#savedPole (shared): the feature's statements; "…" marks code left out
export const savedPole = (p: UtilityPole): NonNullable<SerializedDoc['poles']>[number] => ({ id: p.id, x: p.x, y: p.y, lamp: p.lamp });

// src/world/doc.ts:3691-3691 slice of src/world/doc.ts#savedSpan (shared): the feature's statements; "…" marks code left out
export const savedSpan = (s: UtilitySpan): NonNullable<SerializedDoc['poleSpans']>[number] => ({ id: s.id, a: s.a, b: s.b });

// src/world/doc.ts:3695-3695 slice of src/world/doc.ts#savedBarrier (shared): the feature's statements; "…" marks code left out
export const savedBarrier = (b: Barrier): NonNullable<SerializedDoc['barriers']>[number] => ({ id: b.id, kind: b.kind, points: b.points.map((p) => ({ x: p.x, y: p.y })) });
