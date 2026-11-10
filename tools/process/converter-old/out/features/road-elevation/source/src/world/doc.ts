// The code of src/world/doc.ts that is the feature's (the rest of the file is shared).

// src/world/doc.ts:124-144 slice of src/world/doc.ts#RoadSegment (shared): the feature's statements; "…" marks code left out
export interface RoadSegment {
  // … 16 lines
  structure: RoadStructure;
// … 2 lines
}

// src/world/doc.ts:220-220 slice of src/world/doc.ts#RoadDoc.economy (shared): the feature's statements; "…" marks code left out
  economy: Economy = DEFAULT_ECONOMY;

// src/world/doc.ts:723-730 slice of src/world/doc.ts#RoadDoc.connects (shared): the feature's statements; "…" marks code left out
  connects(a: NodeId, b: NodeId, structure: RoadStructure): boolean {
    // … 2 lines
    return node.incident.some((id) => {
      // … 1 lines
      return s !== undefined && s.structure === structure && ((s.a === a && s.b === b) || (s.a === b && s.b === a));
    });
  }

// src/world/doc.ts:732-765 slice of src/world/doc.ts#RoadDoc.addSegment (shared): the feature's statements; "…" marks code left out
  addSegment(
    // … 7 lines
    structure: RoadStructure = 'ground',
  // … 2 lines
  ): RoadSegment | null {
    // … 6 lines
    const s: RoadSegment = {
      // … 2 lines
      structure,
    // … 2 lines
    };
  // … 8 lines
  }

// src/world/doc.ts:1050-1055 slice of src/world/doc.ts#RoadDoc.setSegmentStructure (shared): the feature's statements; "…" marks code left out
  setSegmentStructure(id: SegmentId, structure: RoadStructure): void {
    // … 1 lines
    if (!was || was.structure === structure) return;
    this.segmentToWrite(id)!.structure = structure;
  // … 1 lines
  }

// src/world/doc.ts:1057-1064 slice of src/world/doc.ts#RoadDoc.setNodeHeightOffset (shared): the feature's statements; "…" marks code left out
  setNodeHeightOffset(id: NodeId, heightOffset: number): void {
    // … 2 lines
    const junctionModeChanged = (was.heightOffset < -TUNNEL_HEADROOM) !==
      (heightOffset < -TUNNEL_HEADROOM);
    // … 1 lines
    this.markNode(id, junctionModeChanged);
  }

// src/world/doc.ts:1155-1159 slice of src/world/doc.ts#RoadDoc.setBalance (shared): the feature's statements; "…" marks code left out
  setBalance(balance: number, cause?: string): void {
    if (!Number.isFinite(balance) || balance === this.economy.balance) return;
    this.economy = { balance };
  // … 1 lines
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
    // … 129 lines
    if (economy) { doc.economy = economy; doc.changes.record('economy', null, { cause: 'map loaded' }); }
  // … 55 lines
  }

// src/world/doc.ts:3029-3047 slice of src/world/doc.ts#segmentRecord (shared): the feature's statements; "…" marks code left out
function segmentRecord(s: SerializedDoc['segments'][number], a: NodeId, b: NodeId): RoadSegment {
  // … 2 lines
  return {
    // … 11 lines
    structure: migrateStructure(s.structure) ?? 'ground',
  // … 1 lines
  };
}

// src/world/doc.ts:3181-3255 slice of src/world/doc.ts#SerializedDoc (shared): the feature's statements; "…" marks code left out
export interface SerializedDoc {
  // … 18 lines
  readonly segments: readonly {
    id: number;
    a: number;
    b: number;
    type: number;
    curve: CurveShape | null;
    dashOrigin?: number;
    direction?: SegmentDirection;
    lanes?: number | null;
    section?: RoadSection;
    parking?: SegmentParking;
    /** A current structure id, or a legacy one `migrateStructure` maps. */
    structure?: RoadStructure | 'viaduct';
    /** Retaining walls in its cutting; absent on every older map. */
    cutWalls?: boolean;
  }[];
// … 39 lines
}

// src/world/doc.ts:3371-3375 slice of src/world/doc.ts#nodeMovesTraffic (shared): the feature's statements; "…" marks code left out
function nodeMovesTraffic(p: RoadNode | undefined, q: RoadNode | undefined): boolean {
  // … 1 lines
  if ((p.heightOffset < -TUNNEL_HEADROOM) !== (q.heightOffset < -TUNNEL_HEADROOM)) return true;
// … 1 lines
}
