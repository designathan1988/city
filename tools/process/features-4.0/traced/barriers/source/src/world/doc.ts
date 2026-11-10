// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:169-169 slice of src/world/doc.ts#RoadDoc.barrierMap (shared): the feature's statements; "…" marks code left out
  private barrierMap: Map<number, Barrier> = new RecordMap();

// src/world/doc.ts:170-170 slice of src/world/doc.ts#RoadDoc.barriers (shared): the feature's statements; "…" marks code left out
  get barriers(): ReadonlyMap<number, Barrier> { return this.barrierMap; }

// src/world/doc.ts:172-172 slice of src/world/doc.ts#RoadDoc.barrierCells (shared): the feature's statements; "…" marks code left out
  private barrierCells = new CellFiling<number, Barrier>(LANDSCAPE_CELL);

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

// src/world/doc.ts:1853-1899 slice of src/world/doc.ts#RoadDoc.layOver (shared): the feature's statements; "…" marks code left out
  private layOver(copy: RoadDoc): void {
    // … 9 lines
    copy.barrierCells = this.barrierCells;
  // … 35 lines
  }

// src/world/doc.ts:1902-1950 slice of src/world/doc.ts#RoadDoc.takeFork (shared): the feature's statements; "…" marks code left out
  private takeFork(copy: RoadDoc): void {
    // … 10 lines
    (copy.barrierMap as ForkMap<number, Barrier>).applyTo(this.barrierMap, (_, barrier) => this.putBarrierRecord(barrier), (id) => this.deleteBarrierRecord(id));
  // … 36 lines
  }

// src/world/doc.ts:1987-2000 slice of src/world/doc.ts#RoadDoc.ownIndex (shared): the feature's statements; "…" marks code left out
  private ownIndex(kind: IndexKind): void {
    // … 1 lines
    switch (kind) {
      // … 3 lines
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
        // … 11 lines
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
        // … 6 lines
        if (part === 'barriers') { const b = this.barriers.get(id); if (b) barriers.push(rectAround(b.points, ITEM_PAD)!); }
      }
    };

// src/world/doc.ts:2404-2552 slice of src/world/doc.ts#RoadDoc.replaceContents (shared): the feature's statements; "…" marks code left out
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
  // … 1 lines
  } {
    // … 27 lines
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
      // … 20 lines
      ...(withRecords && this.barriers.size > 0 ? { barriers: inIdOrder(this.barriers.values()).map(savedBarrier) } : {}),
    // … 10 lines
    };
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 138 lines
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
  // … 21 lines
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

// src/world/doc.ts:3695-3695 slice of src/world/doc.ts#savedBarrier (shared): the feature's statements; "…" marks code left out
export const savedBarrier = (b: Barrier): NonNullable<SerializedDoc['barriers']>[number] => ({ id: b.id, kind: b.kind, points: b.points.map((p) => ({ x: p.x, y: p.y })) });
