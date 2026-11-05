// The code of src/sim/agents/buildingOrder.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/buildingOrder.ts:25-28 slice of src/sim/agents/buildingOrder.ts#StoreOrder.rank (shared): the feature's statements; "…" marks code left out
  rank(id: BuildingId): number | undefined {
    const slot = this.slots.get(id);
    return slot < 0 ? undefined : this.ranks[slot];
  }

// src/sim/agents/buildingOrder.ts:30-32 slice of src/sim/agents/buildingOrder.ts#StoreOrder.has (shared): the feature's statements; "…" marks code left out
  private has(id: BuildingId): boolean {
    return this.slots.get(id) >= 0;
  }

// src/sim/agents/buildingOrder.ts:34-38 slice of src/sim/agents/buildingOrder.ts#StoreOrder.place (shared): the feature's statements; "…" marks code left out
  private place(id: BuildingId): void {
    // … 1 lines
    const slot = this.slots.take(id);
    this.ranks[slot] = this.next++;
  }

// src/sim/agents/buildingOrder.ts:41-49 slice of src/sim/agents/buildingOrder.ts#StoreOrder.follow (shared): the feature's statements; "…" marks code left out
  follow(w: BuildingSource): void {
    // … 5 lines
    if (entries && entries.every((entry) => this.apply(entry.detail, entry.ids as readonly BuildingId[] | undefined))) return;
  // … 1 lines
  }

// src/sim/agents/buildingOrder.ts:52-90 slice of src/sim/agents/buildingOrder.ts#StoreOrder.apply (shared): the feature's statements; "…" marks code left out
  private apply(detail: string | undefined, ids: readonly BuildingId[] | undefined): boolean {
    if (!ids) return false;
    // … 1 lines
    if (detail?.startsWith(BATCH_DETAIL)) {
      // … 1 lines
      if (ops.length !== ids.length) return false;
      for (let k = 0; k < ids.length; k++) {
        const id = ids[k]!, op = ops[k];
        if (op === BATCH_KEPT) { if (!this.has(id)) return false; }
        else if (op === BATCH_GONE) this.slots.delete(id);
        else this.place(id);
      }
    // … 1 lines
    }
    switch (detail) {
      case 'added':
        for (const id of ids) {
          if (this.has(id)) return false;
          this.place(id);
        }
        return true;
      case 'replaced':
        // … 1 lines
        return ids.every((id) => this.has(id));
      case 'removed':
        for (const id of ids) this.slots.delete(id);
        return true;
      case 'restored':
        // … 1 lines
        if (ids.length === 2 && ids[0] === ids[1]) return this.has(ids[0]!);
        if (ids.length !== 1) return false;
        // … 1 lines
        if (this.has(ids[0]!)) this.slots.delete(ids[0]!);
        else this.place(ids[0]!);
        return true;
    // … 2 lines
    }
  }

// src/sim/agents/buildingOrder.ts:102-107 slice of src/sim/agents/buildingOrder.ts#BuildingDelta (shared): the feature's statements; "…" marks code left out
export interface BuildingDelta {
  // … 1 lines
  readonly ids: readonly BuildingId[];
// … 2 lines
}

// src/sim/agents/buildingOrder.ts:125-125 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.filedAt (shared): the feature's statements; "…" marks code left out
  private readonly filedAt = new Map<BuildingId, number>();

// src/sim/agents/buildingOrder.ts:126-126 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.list (shared): the feature's statements; "…" marks code left out
  private list: BuildingId[] = [];

// src/sim/agents/buildingOrder.ts:133-135 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.ids (shared): the feature's statements; "…" marks code left out
  get ids(): readonly BuildingId[] {
    return this.list;
  }

// src/sim/agents/buildingOrder.ts:137-139 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.has (shared): the feature's statements; "…" marks code left out
  has(id: BuildingId): boolean {
    return this.filedAt.has(id);
  }

// src/sim/agents/buildingOrder.ts:142-144 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.rank (shared): the feature's statements; "…" marks code left out
  rank(id: BuildingId): number {
    return this.order.rank(id) ?? Infinity;
  }

// src/sim/agents/buildingOrder.ts:147-183 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.update (shared): the feature's statements; "…" marks code left out
  update(w: BuildingSource): BuildingDelta | null {
    // … 11 lines
    const ids = new Set<BuildingId>();
    // … 2 lines
    for (const entry of entries ?? []) {
      // … 2 lines
      for (const id of entry.ids) ids.add(id as BuildingId);
    }
    // … 6 lines
    if (ids.size > MERGE_PAST) this.merge(w, ids);
    else {
      for (const id of ids) this.unfile(id);
      if (order.epoch !== this.epoch) this.resort();
      for (const id of ids) {
        const b = w.buildings.facts(id);
        if (b && this.keep(b)) this.file(id, order.rank(id)!);
      }
    }
    return { ids: [...ids], rects };
  }

// src/sim/agents/buildingOrder.ts:186-210 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.merge (shared): the feature's statements; "…" marks code left out
  private merge(w: BuildingSource, ids: ReadonlySet<BuildingId>): void {
    // … 1 lines
    for (const id of ids) this.filedAt.delete(id);
    let list = this.list.filter((id) => !ids.has(id));
    if (order.epoch !== this.epoch) {
      list.sort((a, b) => order.rank(a)! - order.rank(b)!);
      for (const id of list) this.filedAt.set(id, order.rank(id)!);
    // … 1 lines
    }
    const come: BuildingId[] = [];
    for (const id of ids) {
      const b = w.buildings.facts(id);
      if (b && this.keep(b)) { come.push(id); this.filedAt.set(id, order.rank(id)!); }
    }
    if (come.length) {
      come.sort((a, b) => this.filedAt.get(a)! - this.filedAt.get(b)!);
      const out: BuildingId[] = new Array(list.length + come.length);
      let i = 0, j = 0, k = 0;
      while (i < list.length || j < come.length) {
        out[k++] = j >= come.length || (i < list.length && this.filedAt.get(list[i]!)! < this.filedAt.get(come[j]!)!) ? list[i++]! : come[j++]!;
      }
      list = out;
    }
    this.list = list;
  }

// src/sim/agents/buildingOrder.ts:213-220 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.slot (shared): the feature's statements; "…" marks code left out
  private slot(rank: number): number {
    let lo = 0, hi = this.list.length;
    while (lo < hi) {
      const mid = (lo + hi) >> 1;
      if (this.filedAt.get(this.list[mid]!)! < rank) lo = mid + 1; else hi = mid;
    }
    return lo;
  }

// src/sim/agents/buildingOrder.ts:222-225 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.file (shared): the feature's statements; "…" marks code left out
  private file(id: BuildingId, rank: number): void {
    this.list.splice(this.slot(rank), 0, id);
    this.filedAt.set(id, rank);
  }

// src/sim/agents/buildingOrder.ts:227-232 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.unfile (shared): the feature's statements; "…" marks code left out
  private unfile(id: BuildingId): void {
    const rank = this.filedAt.get(id);
    if (rank === undefined) return;
    this.list.splice(this.slot(rank), 1);
    this.filedAt.delete(id);
  }

// src/sim/agents/buildingOrder.ts:235-240 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.resort (shared): the feature's statements; "…" marks code left out
  private resort(): void {
    // … 1 lines
    this.list.sort((a, b) => order.rank(a)! - order.rank(b)!);
    for (const id of this.list) this.filedAt.set(id, order.rank(id)!);
  // … 1 lines
  }

// src/sim/agents/buildingOrder.ts:243-251 slice of src/sim/agents/buildingOrder.ts#BuildingsInOrder.reread (shared): the feature's statements; "…" marks code left out
  private reread(w: BuildingSource): void {
    // … 1 lines
    this.list = [];
    w.buildings.eachFacts((b) => {
      // … 2 lines
      this.list.push(b.id);
    });
  }
