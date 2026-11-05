// The code of src/sim/host/simClient.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simClient.ts:197-197 slice of src/sim/host/simClient.ts#SimClient.paused (shared): the feature's statements; "…" marks code left out
  get paused(): boolean { return this.clock.paused; }

// src/sim/host/simClient.ts:336-357 slice of src/sim/host/simClient.ts#SimClient.frame (shared): the feature's statements; "…" marks code left out
  frame(wall: number, hold: boolean, run: { readonly traffic: boolean; readonly pedestrians: boolean; readonly maxShare: number; readonly startShare?: number; readonly opening?: boolean }): void {
    // … 10 lines
    const controls: SimControls = {
      speed: this.clock.speed, paused: this.clock.paused, traffic: run.traffic, pedestrians: run.pedestrians,
      trafficIntensity: this.trafficIntensity, pedestrianIntensity: this.pedestrianIntensity, demandMultiplier: this.demandMultiplier,
      maxShare: run.maxShare, ...(run.startShare !== undefined ? { startShare: run.startShare } : {}), focus: this.focus,
    // … 2 lines
    };
  // … 3 lines
  }

// src/sim/host/simClient.ts:395-398 method SimClient.ask: the feature's own
  ask(ask: SimAsk): Promise<unknown> {
    const id = this.nextAsk++;
    return new Promise((done) => { this.pending.set(id, done); this.send({ type: 'ask', id, ask }); });
  }

// src/sim/host/simClient.ts:404-406 method SimClient.junction: the feature's own
  junction(node: NodeId): JunctionAnswer | null {
    return this.asked({ kind: 'junction', node }) as JunctionAnswer | null;
  }
