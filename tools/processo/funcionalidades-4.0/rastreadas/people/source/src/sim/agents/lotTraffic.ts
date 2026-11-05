// The code of src/sim/agents/lotTraffic.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lotTraffic.ts:678-700 slice of src/sim/agents/lotTraffic.ts#LotTraffic.mayGo (shared): the feature's statements; "…" marks code left out
  private mayGo(w: SimWorld, car: LotCar, body: Vehicle): boolean {
    // … 16 lines
    const people = w.pedEngine.bridge;
    for (const d of discs) if (people.anyoneWithin(w, d.x, d.y, r, null)) { clear = false; break; }
    if (clear || car.waited >= ASK_WAY) { car.reserved = true; return true; }
  // … 2 lines
  }
