// The code of src/sim/city/city.ts that is the feature's (the rest of the file is shared).

// src/sim/city/city.ts:16-16 slice of src/sim/city/city.ts#City.lots (shared): the feature's statements; "…" marks code left out
  readonly lots = new LotTraffic();

// src/sim/city/city.ts:36-41 slice of src/sim/city/city.ts#City.step (shared): the feature's statements; "…" marks code left out
  step(w: SimWorld): void {
    // … 3 lines
    this.lots.step(w);
  }
