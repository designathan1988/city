// The code of src/sim/ambient/ambientIndex.ts that is the feature's (the rest of the file is shared).

// src/sim/ambient/ambientIndex.ts:71-79 slice of src/sim/ambient/ambientIndex.ts#SightWalls.sync (shared): the feature's statements; "…" marks code left out
  sync(store: TrafficBuildings, journal: ChangeJournal): void {
    // … 2 lines
    if (changed === null) { this.fileAll(store); return; }
    for (const id of changed) {
      // … 1 lines
      this.file(id, store.footprints(id as BuildingId));
    }
  }

// src/sim/ambient/ambientIndex.ts:82-90 slice of src/sim/ambient/ambientIndex.ts#SightWalls.fileAll (shared): the feature's statements; "…" marks code left out
  private fileAll(store: TrafficBuildings): void {
    // … 6 lines
    store.eachFootprints((id, rings) => this.file(id, rings));
  }

// src/sim/ambient/ambientIndex.ts:238-247 slice of src/sim/ambient/ambientIndex.ts#BuildingMix.sync (shared): the feature's statements; "…" marks code left out
  sync(store: TrafficBuildings, journal: ChangeJournal): void {
    // … 2 lines
    if (changed === null) { this.fileAll(store); return; }
    for (const id of changed) {
      // … 1 lines
      const b = store.facts(id as never);
      if (b) this.file(b);
    }
  }

// src/sim/ambient/ambientIndex.ts:250-257 slice of src/sim/ambient/ambientIndex.ts#BuildingMix.fileAll (shared): the feature's statements; "…" marks code left out
  private fileAll(store: TrafficBuildings): void {
    // … 5 lines
    store.eachFacts((b) => this.file(b));
  }

// src/sim/ambient/ambientIndex.ts:259-274 slice of src/sim/ambient/ambientIndex.ts#BuildingMix.file (shared): the feature's statements; "…" marks code left out
  private file(b: TrafficFacts): void {
    // … 1 lines
    const placed = Number.isFinite(b.x) && Number.isFinite(b.y);
    const kind = b.kind;
    const slot = this.slots.take(b.id);
    this.xs[slot] = b.x; this.ys[slot] = b.y; this.kinds[slot] = MIX_KINDS.indexOf(kind);
    this.all[kind]++; this.all.total++;
    if (!placed) { this.cellOf[slot] = Number.NaN; this.nowhere[kind]++; this.nowhere.total++; return; }
    const cell = cellKey(Math.floor(b.x / MIX_CELL), Math.floor(b.y / MIX_CELL));
    this.cellOf[slot] = cell;
    let c = this.cells.get(cell);
    if (!c) this.cells.set(cell, c = { counts: noCounts(), members: [] });
    c.counts[kind]++; c.counts.total++;
    this.memberAt[slot] = c.members.length;
    c.members.push(slot);
  }
