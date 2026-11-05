// The code of src/sim/host/simHost.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simHost.ts:202-281 slice of src/sim/host/simHost.ts#SimHost.publish (shared): the feature's statements; "…" marks code left out
  private publish(alpha: number): void {
    // … 73 lines
    this.post({
      type: 'tick', slot, vehicles, peds, ...this.signals(), trains: sim.city.transit.trains().map((t) => ({ colour: t.colour, metro: t.metro, cars: t.cars })),
    // … 2 lines
    }, transfer.length ? transfer : undefined);
  }
