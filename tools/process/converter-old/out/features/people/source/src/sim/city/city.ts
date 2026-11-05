// The code of src/sim/city/city.ts that is the feature's (the rest of the file is shared).

// src/sim/city/city.ts:36-41 slice of src/sim/city/city.ts#City.step (shared): the feature's statements; "…" marks code left out
  step(w: SimWorld): void {
    w.edgeTraffic = !(w.ambient.enabled && w.ambient.source === 'view');
  // … 3 lines
  }
