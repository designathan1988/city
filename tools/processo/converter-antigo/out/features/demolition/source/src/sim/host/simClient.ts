// The code of src/sim/host/simClient.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simClient.ts:229-229 slice of src/sim/host/simClient.ts#SimClient.casualtyList (shared): the feature's statements; "…" marks code left out
  private casualtyList: Casualty[] = [];

// src/sim/host/simClient.ts:244-248 method SimClient.takeWounds: the feature's own
  takeWounds(): Wound[] {
    const out = this.wounds;
    this.wounds = [];
    return out;
  }

// src/sim/host/simClient.ts:251-255 method SimClient.casualties: the feature's own
  casualties(dt = 0): readonly Casualty[] {
    for (const c of this.casualtyList) c.t += dt;
    if (this.casualtyList.length && this.casualtyList[0]!.t > 60) this.casualtyList = this.casualtyList.filter((c) => c.t <= 60);
    return this.casualtyList;
  }

// src/sim/host/simClient.ts:367-370 method SimClient.act: the feature's own
  act(act: SimAct): Promise<unknown> {
    const id = this.nextAsk++;
    return new Promise((done) => { this.pending.set(id, done); this.send({ type: 'act', id, act }); });
  }

// src/sim/host/simClient.ts:548-707 slice of src/sim/host/simClient.ts#SimClient.readTick (shared): the feature's statements; "…" marks code left out
  private readTick(message: Extract<FromSim, { type: 'tick' }>): void {
    // … 137 lines
    if (message.casualties.length) this.casualtyList.push(...message.casualties);
  // … 20 lines
  }
