// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:187-187 slice of src/world/doc.ts#RoadDoc.transit (shared): the feature's statements; "…" marks code left out
  transit: TransitData = emptyTransit();

// src/world/doc.ts:190-192 slice of src/world/doc.ts#RoadDoc.setTransit (shared): the feature's statements; "…" marks code left out
  setTransit(next: TransitData): void {
    this.transit = next;    this.changes.record('transit', null);
  }

// src/world/doc.ts:380-380 getter RoadDoc.transitRevision: the feature's own
  get transitRevision(): number { return this.changes.serialOf('transit'); }

// src/world/doc.ts:1853-1899 slice of src/world/doc.ts#RoadDoc.layOver (shared): the feature's statements; "…" marks code left out
  private layOver(copy: RoadDoc): void {
    // … 19 lines
    copy.transit = this.transit;
  // … 25 lines
  }

// src/world/doc.ts:1902-1950 slice of src/world/doc.ts#RoadDoc.takeFork (shared): the feature's statements; "…" marks code left out
  private takeFork(copy: RoadDoc): void {
    // … 27 lines
    this.transit = copy.transit;
  // … 19 lines
  }

// src/world/doc.ts:2358-2372 slice of src/world/doc.ts#RoadDoc.valueOf (shared): the feature's statements; "…" marks code left out
  valueOf(part: ValuePart): unknown {
    switch (part) {
      // … 7 lines
      case 'transit': return this.transit;
    // … 3 lines
    }
  }

// src/world/doc.ts:2374-2400 slice of src/world/doc.ts#RoadDoc.restoreValue (shared): the feature's statements; "…" marks code left out
  private restoreValue(part: ValuePart, raw: unknown, kinds: Set<DocChangeKind>): void {
    // … 1 lines
    switch (part) {
      // … 7 lines
      case 'transit': this.transit = value as TransitData; kinds.add('transit'); return;
    // … 14 lines
    }
  }

// src/world/doc.ts:2404-2552 slice of src/world/doc.ts#RoadDoc.replaceContents (shared): the feature's statements; "…" marks code left out
  private replaceContents(source: RoadDoc, moved: MovedKinds, buildingIds: readonly BuildingId[] | null, lotIds: readonly number[] | null, roadsDiffer: boolean, whole = false): {
  // … 1 lines
  } {
    // … 49 lines
    if (moved('transit') && (whole || !sameText(this.transit, source.transit))) {
      this.transit = JSON.parse(JSON.stringify(source.transit)) as TransitData;
      others.push('transit');
    }
  // … 92 lines
  }

// src/world/doc.ts:2599-2616 slice of src/world/doc.ts#RoadDoc.savedGlobals (shared): the feature's statements; "…" marks code left out
  savedGlobals(part: GlobalPart): Partial<SavedGlobals> {
    switch (part) {
      // … 13 lines
      case 'transit': return hasTransit(this.transit) ? { transit: structuredClone(this.transit) } : {};
    }
  }

// src/world/doc.ts:2673-2698 slice of src/world/doc.ts#RoadDoc.worldPart (shared): the feature's statements; "…" marks code left out
  worldPart(part: WorldPart): Partial<SerializedDoc> {
    switch (part) {
      // … 18 lines
      case 'transit': return hasTransit(this.transit) ? { transit: JSON.parse(JSON.stringify(this.transit)) as TransitData } : {};
    // … 3 lines
    }
  }

// src/world/doc.ts:2701-2887 slice of src/world/doc.ts#RoadDoc.fromJSON (shared): the feature's statements; "…" marks code left out
  static fromJSON(saved: SerializedDoc, options: { readonly repair?: boolean; readonly shareBuildings?: BuildingStore; readonly preparedBuildings?: boolean; readonly buildingSections?: BuildingSections } = {}): RoadDoc {
    // … 126 lines
    if (data.transit) doc.transit = normalizeTransit(data.transit);
  // … 58 lines
  }
