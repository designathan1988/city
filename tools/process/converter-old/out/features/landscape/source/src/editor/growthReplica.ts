// The code of src/editor/growthReplica.ts that is the feature's (the rest of the file is shared).

// src/editor/growthReplica.ts:63-92 slice of src/editor/growthReplica.ts#GrowAsk (shared): the feature's statements; "…" marks code left out
export interface GrowAsk {
  // … 19 lines
  readonly ground: GroundPatch | null;
// … 8 lines
}

// src/editor/growthReplica.ts:251-289 slice of src/editor/growthReplica.ts#GrowthReplica.handle (shared): the feature's statements; "…" marks code left out
  handle(ask: GrowAsk): GrowAnswer {
    // … 5 lines
    const groundAt = ask.ground ? patchGround(ask.ground) : null;
    // … 5 lines
    for (;;) {
      const site: GrowthSite = { neighbours: this.neighbours, net: ask.roads ? this.roads : null, groundAt, ...(pavedAt ? { pavedAt } : {}), nextId: nextId as BuildingId };
      // … 1 lines
      try {
      // … 1 lines
      } catch (error) {
        if (!(error instanceof GrowthMiss || error instanceof PatchMiss)) throw error;
      // … 1 lines
      }
    // … 15 lines
    }
  // … 1 lines
  }
