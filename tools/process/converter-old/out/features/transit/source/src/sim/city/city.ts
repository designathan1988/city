// The code of src/sim/city/city.ts that is the feature's (the rest of the file is shared).

// src/sim/city/city.ts:14-14 slice of src/sim/city/city.ts#City.transit (shared): the feature's statements; "…" marks code left out
  readonly transit = new TransitSim();

// src/sim/city/city.ts:36-41 slice of src/sim/city/city.ts#City.step (shared): the feature's statements; "…" marks code left out
  step(w: SimWorld): void {
    // … 1 lines
    this.transit.step(w);
  // … 2 lines
  }
