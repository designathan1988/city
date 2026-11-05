// The code of src/editor/growthReplica.ts that is the feature's (the rest of the file is shared).

// src/editor/growthReplica.ts:63-92 slice of src/editor/growthReplica.ts#GrowAsk (shared): the feature's statements; "…" marks code left out
export interface GrowAsk {
  // … 21 lines
  readonly paving: PavingData | null;
// … 6 lines
}

// src/editor/growthReplica.ts:170-170 slice of src/editor/growthReplica.ts#RoadReplica.kerbs (shared): the feature's statements; "…" marks code left out
  private readonly kerbs = new Map<NodeId, ReadonlyMap<SurfaceLevel, { readonly ring: JunctionRing }>>();

// src/editor/growthReplica.ts:195-219 slice of src/editor/growthReplica.ts#RoadReplica.apply (shared): the feature's statements; "…" marks code left out
  apply(sync: NonNullable<GrowthSync['roads']>): void {
    // … 14 lines
    for (const piece of sync.junctions) {
      // … 4 lines
      const byLevel = new Map<SurfaceLevel, { readonly ring: JunctionRing }>([[Level.Curb, { ring }]]);
      this.kerbs.set(node, byLevel);
      this.kerbCells.file(node, byLevel, piece.rect);
    }
  }

// src/editor/growthReplica.ts:234-243 slice of src/editor/growthReplica.ts#pavedLookup (shared): the feature's statements; "…" marks code left out
function pavedLookup(paved: Float64Array | null, paving: PavingData | null): (x: number, y: number) => number {
  // … 2 lines
  const read = paving ? pavingReader(paving) : null;
  return (x, y) => {
    const h = at.get(`${x},${y}`) ?? read?.(x, y);
    if (h === undefined) throw new GrowthMiss(`the paving asked at (${x}, ${y}), not sent`);
    return h;
  };
}
