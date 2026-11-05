// The code of src/sim/host/simClient.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simClient.ts:387-392 slice of src/sim/host/simClient.ts#SimClient.lotPlan (shared): the feature's statements; "…" marks code left out
  lotPlan(): Promise<LotPlan | null> {
    // … 3 lines
    return new Promise((done) => { this.pending.set(id, (value) => done((value as LotPlan | null) ?? null)); this.send({ type: 'lotPlan', id }); });
  }

// src/sim/host/simClient.ts:432-439 slice of src/sim/host/simClient.ts#SimClient.markSent (shared): the feature's statements; "…" marks code left out
  private markSent(): void {
    // … 4 lines
    this.sentKeys = this.doc.lotStore.keys.slice();
    this.lotKeysRewritten = this.doc.lotStore.keysRewritten;
  }

// src/sim/host/simClient.ts:442-506 slice of src/sim/host/simClient.ts#SimClient.sendChanges (shared): the feature's statements; "…" marks code left out
  private sendChanges(): void {
    // … 10 lines
    const buildings = new Set<BuildingId>();
    // … 2 lines
    for (const e of entries) {
      if (e.kind === 'buildings') {
        // … 2 lines
        for (const id of e.ids) buildings.add(id as BuildingId);
      // … 1 lines
      }
    // … 13 lines
    }
    // … 11 lines
    const buildingList = [...buildings];
    // … 1 lines
    if (keys) {
      const store = this.doc.lotStore, all = store.keys;
      // … 1 lines
      const sent = this.sentKeys;
      if (store.keysRewritten === this.lotKeysRewritten && all.length >= sent.length) {
        // Only added at the end since: the new tail.
        const items = all.slice(sent.length);
        lists.push({ part: 'lotKeys', start: sent.length, remove: 0, items });
        for (const key of items) sent.push(key);
      } else lists.push(...this.keyChanges());
      this.lotKeysRewritten = store.keysRewritten;
    }
    this.send({
      // … 1 lines
      buildings: buildingChangesOf(this.doc, buildingList),
    });
  }

// src/sim/host/simClient.ts:509-525 slice of src/sim/host/simClient.ts#SimClient.keyChanges (shared): the feature's statements; "…" marks code left out
  private keyChanges(): Restoration['lists'][number][] {
    const store = this.doc.lotStore, sent = this.sentKeys;
    // … 1 lines
    for (let n = sent.length - 1; n >= 0; n--) {
      if (store.hasKey(sent[n]!)) continue;
      let m = n;
      while (m > 0 && !store.hasKey(sent[m - 1]!)) m--;
      out.push({ part: 'lotKeys', start: m, remove: n - m + 1, items: [] });
      n = m;
    }
    const kept = sent.filter((key) => store.hasKey(key));
    const held = new Set(kept);
    const added = store.keys.filter((key) => !held.has(key));
    if (added.length) out.push({ part: 'lotKeys', start: kept.length, remove: 0, items: added });
    this.sentKeys = [...kept, ...added];
  // … 1 lines
  }

// src/sim/host/simClient.ts:789-796 slice of src/sim/host/simClient.ts#recordOf (shared): the feature's statements; "…" marks code left out
function recordOf(doc: RoadDoc, part: RecordPart, id: number): unknown {
  switch (part) {
    // … 2 lines
    case 'lots': return doc.lotStore.get(id);
  // … 1 lines
  }
}
