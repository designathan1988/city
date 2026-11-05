// The code of src/sim/host/simHost.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simHost.ts:404-409 slice of src/sim/host/simHost.ts#SimHost.lotPlan (shared): the feature's statements; "…" marks code left out
  private lotPlan(): LotPlan | null {
    // … 3 lines
    return planLots(this.doc, this.net);
  }
