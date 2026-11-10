// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:220-220 slice of src/world/doc.ts#RoadDoc.economy (shared): the feature's statements; "…" marks code left out
  economy: Economy = DEFAULT_ECONOMY;

// src/world/doc.ts:273-276 getter RoadDoc.forkOrigin: the feature's own
  get forkOrigin(): RoadDoc | null {
    const fork = this.forkedFrom;
    return fork && fork.doc.changes.version === fork.version ? fork.doc : null;
  }

// src/world/doc.ts:1155-1159 method RoadDoc.setBalance: the feature's own
  setBalance(balance: number, cause?: string): void {
    if (!Number.isFinite(balance) || balance === this.economy.balance) return;
    this.economy = { balance };
    this.changes.record('economy', null, cause ? { cause } : {});
  }

// src/world/doc.ts:1853-1899 slice of src/world/doc.ts#RoadDoc.layOver (shared): the feature's statements; "…" marks code left out
  private layOver(copy: RoadDoc): void {
    // … 26 lines
    copy.economy = this.economy;
  // … 18 lines
  }

// src/world/doc.ts:1902-1950 slice of src/world/doc.ts#RoadDoc.takeFork (shared): the feature's statements; "…" marks code left out
  private takeFork(copy: RoadDoc): void {
    // … 34 lines
    this.economy = copy.economy;
  // … 12 lines
  }

// src/world/doc.ts:2358-2372 slice of src/world/doc.ts#RoadDoc.valueOf (shared): the feature's statements; "…" marks code left out
  valueOf(part: ValuePart): unknown {
    switch (part) {
      // … 6 lines
      case 'economy': return this.economy;
    // … 4 lines
    }
  }

// src/world/doc.ts:2374-2400 slice of src/world/doc.ts#RoadDoc.restoreValue (shared): the feature's statements; "…" marks code left out
  private restoreValue(part: ValuePart, raw: unknown, kinds: Set<DocChangeKind>): void {
    // … 1 lines
    switch (part) {
      // … 6 lines
      case 'economy': this.economy = value as Economy; kinds.add('economy'); return;
    // … 15 lines
    }
  }

// src/world/doc.ts:2404-2552 slice of src/world/doc.ts#RoadDoc.replaceContents (shared): the feature's statements; "…" marks code left out
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
  // … 1 lines
  } {
    // … 65 lines
    if (this.economy.balance !== source.economy.balance) {
      this.economy = { ...source.economy };
      others.push('economy');
    }
  // … 76 lines
  }

// src/world/doc.ts:2599-2616 slice of src/world/doc.ts#RoadDoc.savedGlobals (shared): the feature's statements; "…" marks code left out
  savedGlobals(part: GlobalPart): Partial<SavedGlobals> {
    switch (part) {
      case 'values': return {
        // … 8 lines
        ...(this.economy.balance !== DEFAULT_ECONOMY.balance ? { economy: { balance: this.economy.balance } } : {}),
      };
    // … 3 lines
    }
  }

// src/world/doc.ts:2673-2698 slice of src/world/doc.ts#RoadDoc.worldPart (shared): the feature's statements; "…" marks code left out
  worldPart(part: WorldPart): Partial<SerializedDoc> {
    switch (part) {
      // … 20 lines
      case 'economy': return this.economy.balance !== DEFAULT_ECONOMY.balance ? { economy: { balance: this.economy.balance } } : {};
    // … 1 lines
    }
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 128 lines
    const economy = data.economy === undefined ? null : readEconomy(data.economy);
    if (economy) { doc.economy = economy; doc.changes.record('economy', null, { cause: 'map loaded' }); }
  // … 55 lines
  }
