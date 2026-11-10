// The code of src/sim/routing/searchScratch.ts that is the feature's (the rest of the file is shared).

// src/sim/routing/searchScratch.ts:11-11 slice of src/sim/routing/searchScratch.ts#SearchScratch.slots (shared): the feature's statements; "…" marks code left out
  private readonly slots = new Map<LaneletId, number>();

// src/sim/routing/searchScratch.ts:13-13 slice of src/sim/routing/searchScratch.ts#SearchScratch.ids (shared): the feature's statements; "…" marks code left out
  readonly ids: LaneletId[] = [];

// src/sim/routing/searchScratch.ts:52-61 slice of src/sim/routing/searchScratch.ts#SearchScratch.slot (shared): the feature's statements; "…" marks code left out
  slot(id: LaneletId): number {
    let slot = this.slots.get(id);
    if (slot === undefined) {
      slot = this.ids.length;
      this.ids.push(id);
      this.slots.set(id, slot);
      if (slot >= this.bound.length) this.grow(slot + 1);
    }
    return slot;
  }
