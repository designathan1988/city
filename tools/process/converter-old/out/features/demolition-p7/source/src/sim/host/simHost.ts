// The code of src/sim/host/simHost.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simHost.ts:80-80 slice of src/sim/host/simHost.ts#SimHost.casualtiesSent (shared): the feature's statements; "…" marks code left out
  private readonly casualtiesSent = new WeakSet<Casualty>();

// src/sim/host/simHost.ts:113-144 slice of src/sim/host/simHost.ts#SimHost.init (shared): the feature's statements; "…" marks code left out
  private async init(map: TrafficMap, seed: number, zones: ReadonlyMap<string, Float64Array | null>, data: SharedArrayBuffer, control: SharedArrayBuffer,
    prefix: string | null, ambient: 'view' | 'edges', bake: NetworkBake | null): Promise<void> {
    // … 19 lines
    sim.usePedestrianEngine((await import('../agents/walk')).createAgentWalkEngine());
  // … 9 lines
  }

// src/sim/host/simHost.ts:161-199 slice of src/sim/host/simHost.ts#SimHost.frame (shared): the feature's statements; "…" marks code left out
  private frame(wall: number, hold: boolean, controls: SimControls): void {
    // … 15 lines
    if (controls.opening) {
      impactCasualties(sim, wall);
    // … 2 lines
    }
    // … 15 lines
    impactCasualties(sim, wall);
  // … 1 lines
  }

// src/sim/host/simHost.ts:202-281 slice of src/sim/host/simHost.ts#SimHost.publish (shared): the feature's statements; "…" marks code left out
  private publish(alpha: number): void {
    // … 73 lines
    this.post({
      // … 1 lines
      parked, meso, stats: this.stats(), wounds: takeWounds(sim),
      casualties: impactCasualties(sim, 0).filter((c) => !this.casualtiesSent.has(c) && (this.casualtiesSent.add(c), true)),
    }, transfer.length ? transfer : undefined);
  }
