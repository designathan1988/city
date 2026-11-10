// The code of src/sim/host/simClient.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simClient.ts:232-232 slice of src/sim/host/simClient.ts#SimClient.constructor (shared): the feature's statements; "…" marks code left out
  constructor(readonly doc: RoadDoc, private readonly net: Network | null = null) {}

// src/sim/host/simClient.ts:235-241 slice of src/sim/host/simClient.ts#SimClient.bake (shared): the feature's statements; "…" marks code left out
  private bake(): NetworkBake | null {
    // … 1 lines
    if (!net || net.doc !== this.doc) return null;
    // … 1 lines
    if (!net.isBuilt()) net.rebuild();
    return net.exportBake(this.doc.nodes.keys(), this.doc.segments.keys());
  }

// src/sim/host/simClient.ts:395-398 method SimClient.ask: the feature's own
  ask(ask: SimAsk): Promise<unknown> {
    const id = this.nextAsk++;
    return new Promise((done) => { this.pending.set(id, done); this.send({ type: 'ask', id, ask }); });
  }
