// The code of src/sim/agents/lotTraffic.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lotTraffic.ts:619-648 slice of src/sim/agents/lotTraffic.ts#LotTraffic.leaving (shared): the feature's statements; "…" marks code left out
  private leaving(w: SimWorld, car: LotCar): void {
    // … 16 lines
    const v0 = l.speedLimit * a.speedFactor * w.rng.driver.range(DRIVER_NOISE.lo, DRIVER_NOISE.hi);
    const road = createVehicle(id, a, body.driver, body.color, l.id, v0, w.clock.tick);
    road.s = Math.min(l.length, lane.at + a.length / 2);
    road.v = JOIN_SPEED;
    road.prev = snapshot(road);
    // … 1 lines
    assignOccupancy(w, road);
    w.vehicles.set(road.id, road);
    w.enterLanelet(road, l.id);
    road.destination = chooseVehicleDestination(w, l.id, bodyClassOfArchetype(a));
    planFrom(w, road);
  // … 1 lines
  }
