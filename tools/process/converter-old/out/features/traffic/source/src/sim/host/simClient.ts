// The code of src/sim/host/simClient.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simClient.ts:28-52 slice of src/sim/host/simClient.ts#VehicleView (shared): the feature's statements; "…" marks code left out
export interface VehicleView {
  // … 1 lines
  archetype: Archetype;
  // … 2 lines
  kerbStop: KerbStop | null;
// … 18 lines
}

// src/sim/host/simClient.ts:58-66 slice of src/sim/host/simClient.ts#SignalFeed (shared): the feature's statements; "…" marks code left out
export interface SignalFeed {
  // … 4 lines
  state(node: NodeId, group: number): SignalState;
// … 2 lines
}

// src/sim/host/simClient.ts:147-151 method SimClient.parkedCellsIn: the feature's own
  parkedCellsIn(box: Readonly<GroundBox>, out: FleetCell<VehicleView>[]): FleetCell<VehicleView>[] {
    out.length = 0;
    const s = this.parkedSlack;
    return this.parkedGrid.cellsNear(box.x0 - s, box.y0 - s, box.x1 + s, box.y1 + s, out);
  }

// src/sim/host/simClient.ts:158-169 slice of src/sim/host/simClient.ts#SimClient.dropParked (shared): the feature's statements; "…" marks code left out
  private dropParked(view: VehicleView): void {
    // … 9 lines
    this.onVehicleGone?.(view);
  }

// src/sim/host/simClient.ts:172-181 slice of src/sim/host/simClient.ts#SimClient.clearParked (shared): the feature's statements; "…" marks code left out
  private clearParked(): void {
    const gone = this.onVehicleGone;
    if (gone) for (const view of this.parked) gone(view);
  // … 6 lines
  }

// src/sim/host/simClient.ts:207-207 slice of src/sim/host/simClient.ts#SimClient.lights (shared): the feature's statements; "…" marks code left out
  private readonly lights = new Map<NodeId, { stage: string; readonly states: Map<number, SignalState> }>();

// src/sim/host/simClient.ts:259-282 slice of src/sim/host/simClient.ts#SimClient.start (shared): the feature's statements; "…" marks code left out
  start(seed: number, zones: ReadonlyMap<string, Float64Array | null>, zonesPrefix: string | null, ambient: 'view' | 'edges',
    lanes: { readonly bays: readonly MessagePort[]; readonly conflicts: readonly MessagePort[] } | null = null): Promise<void> {
    // … 19 lines
    this.send({ type: 'init', frames: this.frames, map: trafficMapOf(this.doc), seed, zones, zonesPrefix, ambient, bake: this.bake() });
  // … 1 lines
  }

// src/sim/host/simClient.ts:290-294 slice of src/sim/host/simClient.ts#SimClient.load (shared): the feature's statements; "…" marks code left out
  load(): void {
    // … 1 lines
    this.send({ type: 'load', map: trafficMapOf(this.doc), bake: this.bake() });
  // … 1 lines
  }

// src/sim/host/simClient.ts:297-300 slice of src/sim/host/simClient.ts#SimClient.resync (shared): the feature's statements; "…" marks code left out
  private resync(): void {
    // … 1 lines
    this.send({ type: 'resync', map: trafficMapOf(this.doc) });
  }

// src/sim/host/simClient.ts:314-319 method SimClient.takeMeso: the feature's own
  takeMeso(): readonly MesoDelta[] {
    if (!this.mesoDeltas.length) return this.noDeltas;
    const out = this.mesoDeltas;
    this.mesoDeltas = [];
    return out;
  }

// src/sim/host/simClient.ts:320-320 field SimClient.noDeltas: the feature's own
  private readonly noDeltas: readonly MesoDelta[] = [];

// src/sim/host/simClient.ts:323-323 slice of src/sim/host/simClient.ts#SimClient.mesoDeltas (shared): the feature's statements; "…" marks code left out
  private mesoDeltas: MesoDelta[] = [];

// src/sim/host/simClient.ts:327-333 slice of src/sim/host/simClient.ts#SimClient.keepMeso (shared): the feature's statements; "…" marks code left out
  private keepMeso(d: MesoDelta): void {
    if (d.whole) this.mesoDeltas.length = 0;
    this.mesoDeltas.push(d);
  // … 3 lines
  }

// src/sim/host/simClient.ts:395-398 method SimClient.ask: the feature's own
  ask(ask: SimAsk): Promise<unknown> {
    const id = this.nextAsk++;
    return new Promise((done) => { this.pending.set(id, done); this.send({ type: 'ask', id, ask }); });
  }

// src/sim/host/simClient.ts:442-506 slice of src/sim/host/simClient.ts#SimClient.sendChanges (shared): the feature's statements; "…" marks code left out
  private sendChanges(): void {
    // … 59 lines
    this.send({
      // … 1 lines
      buildings: buildingChangesOf(this.doc, buildingList),
    });
  }

// src/sim/host/simClient.ts:548-707 slice of src/sim/host/simClient.ts#SimClient.readTick (shared): the feature's statements; "…" marks code left out
  private readTick(message: Extract<FromSim, { type: 'tick' }>): void {
    // … 30 lines
    for (let k = 0; k < n; k++) {
      // … 10 lines
      if (r) {
        view.archetype = archetypeById(r.archetype);
      // … 5 lines
      }
    // … 30 lines
    }
    // … 1 lines
    if (read !== this.vehicles.size) {
      for (const [id, view] of this.vehicles) if (view.tickRead !== stamp) { this.vehicles.delete(id); this.trafficGrid.remove(view); this.onVehicleGone?.(view); }
    }
  // … 74 lines
  }

// src/sim/host/simClient.ts:710-733 slice of src/sim/host/simClient.ts#SimClient.applyParked (shared): the feature's statements; "…" marks code left out
  private applyParked(told: ParkedChanges, stamp: number): void {
    // … 1 lines
    for (let at = 0; at < cars.length; at += PARKED_STRIDE) {
      // … 4 lines
      view.archetype = ARCHETYPES[cars[at + PK.archetype]!] ?? archetypeById('');
    // … 8 lines
    }
  // … 6 lines
  }

// src/sim/host/simClient.ts:735-754 slice of src/sim/host/simClient.ts#SimClient.clearViews (shared): the feature's statements; "…" marks code left out
  private clearViews(): void {
    const gone = this.onVehicleGone;
    if (gone) for (const view of this.vehicles.values()) gone(view);
  // … 16 lines
  }

// src/sim/host/simClient.ts:770-776 slice of src/sim/host/simClient.ts#newView (shared): the feature's statements; "…" marks code left out
function newView(id: number): VehicleView {
  return {
    id, archetype: archetypeById('car'), color: '#888888', doors: [], kerbStop: null, people: [], peopleAge: [], seats: 0, slain: false, age: 0, v: 0, prev: { v: 0 },
  // … 2 lines
  };
}
