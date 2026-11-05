// The code of src/sim/agents/lorryTraffic.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lorryTraffic.ts:44-67 slice of src/sim/agents/lorryTraffic.ts#Lorry (shared): the feature's statements; "…" marks code left out
interface Lorry {
  // … 1 lines
  readonly building: BuildingId;
// … 20 lines
}

// src/sim/agents/lorryTraffic.ts:73-73 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.planning (shared): the feature's statements; "…" marks code left out
  private planning: { building: BuildingId; dock: number } | null = null;

// src/sim/agents/lorryTraffic.ts:83-89 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.reset (shared): the feature's statements; "…" marks code left out
  reset(w: SimWorld | null = this.world): void {
    // … 2 lines
    this.planning = null;
  // … 2 lines
  }

// src/sim/agents/lorryTraffic.ts:165-165 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.yards (shared): the feature's statements; "…" marks code left out
  private yards: readonly { building: BuildingId; docks: number }[] = [];

// src/sim/agents/lorryTraffic.ts:168-174 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.readYards (shared): the feature's statements; "…" marks code left out
  private readYards(w: SimWorld): void {
    // … 3 lines
    if (gone === null) forgetYardsBut(w, (id) => this.yardList.has(id as BuildingId));
    else for (const id of gone) forgetYard(w, id);
  }

// src/sim/agents/lorryTraffic.ts:180-201 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.yardFor (shared): the feature's statements; "…" marks code left out
  private yardFor(w: SimWorld, building: BuildingId, begin = true): Yard | null | undefined {
    const b = w.buildings.get(building);
    if (!b) return null;
    const known = knownYard(w, b);
    if (known !== undefined) return known;
    if (!begin && this.making?.building !== b) return undefined;
    // … 3 lines
    if (making && making.building === b && making.key !== key) {
      const inputs = yardInputs(w, b);
      if (inputs === making.inputs) making.key = key;
    }
    if (!this.making || this.making.building !== b || this.making.key !== key) {
      this.making = { building: b, key, inputs: yardInputs(w, b), steps: buildYard(b, key, w, lorryOf(ARCHETYPES.find((a) => a.id === 'truck')!)) };
    }
  // … 5 lines
  }

// src/sim/agents/lorryTraffic.ts:204-215 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.plan (shared): the feature's statements; "…" marks code left out
  private plan(w: SimWorld): void {
    const p = this.planning;
    if (!p) return;
    const yard = this.yardFor(w, p.building);
    if (yard === undefined) return;
    if (!yard || !yard.docks[p.dock]) { this.planning = null; return; }
    yard.plan(p.dock, PLAN_BUDGET);
    if (!yard.planned(p.dock)) return;
    // … 1 lines
    const rest = yard.docks.findIndex((_, k) => !yard.planned(k));
    this.planning = rest >= 0 ? { building: p.building, dock: rest } : null;
  }

// src/sim/agents/lorryTraffic.ts:218-262 slice of src/sim/agents/lorryTraffic.ts#LorryTraffic.call (shared): the feature's statements; "…" marks code left out
  private call(w: SimWorld): void {
    // … 16 lines
    const yard = this.yardFor(w, pick.building, this.planning === null || this.planning.building === pick.building);
    if (yard === null) return;
    if (yard === undefined || !yard.planned(dock)) { this.planning ??= { building: pick.building, dock }; return; }
    const ways = yard.ways(dock);
    if (!ways) return;
    const gate = ways.gate;
    const lane = w.lanelet(gate.lane.lanelet);
    if (!lane) return;
    const stop = lane.centre.sampleAt(gate.stopAt).p;
    // … 2 lines
    for (const v of w.vehiclesNear(stop.x, stop.y, CALL_REACH)) {
      if (v.archetype.id !== 'truck' || v.commute || v.kerbStop || v.errand || this.lorries.has(v.id)) continue;
      if (v.lanelet === lane.id && v.s > gate.stopAt) continue;
      const pose = vehiclePose(w, v, 1);
      if (!pose) continue;
      const d = Math.hypot(pose.p.x - stop.x, pose.p.y - stop.y);
      if (d < bestD) { bestD = d; best = v; }
    }
    if (!best) return;
    const v = best, was = v.destination;
    v.commute = { trip: -3, lanelet: lane.id, at: gate.stopAt + v.archetype.length / 2 };
    v.destination = lane.id;
    planFrom(w, v);
    if (v.destination !== lane.id) { v.commute = null; v.destination = was; planFrom(w, v); return; }
    this.lorries.set(v.id, this.make(w, v, yard, dock, ways));
  // … 1 lines
  }

// src/sim/agents/lorryTraffic.ts:460-460 slice of src/sim/agents/lorryTraffic.ts#YardList.docks (shared): the feature's statements; "…" marks code left out
  private readonly docks = new Map<BuildingId, number>();

// src/sim/agents/lorryTraffic.ts:461-461 slice of src/sim/agents/lorryTraffic.ts#YardList.list (shared): the feature's statements; "…" marks code left out
  private list: { building: BuildingId; docks: number }[] = [];

// src/sim/agents/lorryTraffic.ts:463-463 slice of src/sim/agents/lorryTraffic.ts#YardList.gone (shared): the feature's statements; "…" marks code left out
  gone: BuildingId[] | null = [];

// src/sim/agents/lorryTraffic.ts:466-468 slice of src/sim/agents/lorryTraffic.ts#YardList.has (shared): the feature's statements; "…" marks code left out
  has(id: BuildingId): boolean {
    return this.kept.has(id);
  }

// src/sim/agents/lorryTraffic.ts:470-482 slice of src/sim/agents/lorryTraffic.ts#YardList.read (shared): the feature's statements; "…" marks code left out
  read(w: BuildingSource): readonly { building: BuildingId; docks: number }[] {
    // … 2 lines
    if (delta !== null && delta.ids.length === 0) return this.list;
    // … 1 lines
    for (const id of delta === null ? this.kept.ids : delta.ids) {
      const b = w.buildings.get(id);
      if (b && this.kept.has(id)) this.docks.set(id, b.docks.length);
      else { this.docks.delete(id); this.gone?.push(id); }
    }
    this.list = this.kept.ids.map((building) => ({ building, docks: this.docks.get(building)! }));
    return this.list;
  }
