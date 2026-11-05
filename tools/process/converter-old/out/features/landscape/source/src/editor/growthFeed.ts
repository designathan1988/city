// The code of src/editor/growthFeed.ts that is the feature's (the rest of the file is shared).

// src/editor/growthFeed.ts:227-274 slice of src/editor/growthFeed.ts#GrowthSession.send (shared): the feature's statements; "…" marks code left out
  private send(lot: Lot, rng: RngState, refused: Set<number>, extra: number, misses: number): void {
    // … 41 lines
    this.lane.post({
      // … 1 lines
      ground: host.groundAt ? sampleGroundPatch(reach.lot, host.cell, host.groundAt) : null,
    // … 1 lines
    }, transfer);
  }
