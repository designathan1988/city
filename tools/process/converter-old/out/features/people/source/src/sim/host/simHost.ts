// The code of src/sim/host/simHost.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simHost.ts:80-80 slice of src/sim/host/simHost.ts#SimHost.casualtiesSent (shared): the feature's statements; "…" marks code left out
  private readonly casualtiesSent = new WeakSet<Casualty>();

// src/sim/host/simHost.ts:87-111 slice of src/sim/host/simHost.ts#SimHost.handle (shared): the feature's statements; "…" marks code left out
  async handle(message: ToSim): Promise<void> {
    switch (message.type) {
      // … 12 lines
      case 'reset': this.sim?.reset(); this.sim?.ambient.reset(this.sim); return;
    // … 8 lines
    }
  }

// src/sim/host/simHost.ts:113-144 slice of src/sim/host/simHost.ts#SimHost.init (shared): the feature's statements; "…" marks code left out
  private async init(map: TrafficMap, seed: number, zones: ReadonlyMap<string, Float64Array | null>, data: SharedArrayBuffer, control: SharedArrayBuffer,
    prefix: string | null, ambient: 'view' | 'edges', bake: NetworkBake | null): Promise<void> {
    // … 19 lines
    sim.usePedestrianEngine((await import('../agents/walk')).createAgentWalkEngine());
    // … 1 lines
    sim.ambient.enabled = true;
    sim.ambient.source = ambient;
  // … 6 lines
  }

// src/sim/host/simHost.ts:147-158 slice of src/sim/host/simHost.ts#SimHost.load (shared): the feature's statements; "…" marks code left out
  private load(map: TrafficMap, bake: NetworkBake | null): void {
    // … 4 lines
    if (this.sim) this.sim.ambient.reset(this.sim);
    // … 4 lines
    this.parkedSerial = -1;
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
    // … 43 lines
    for (const v of sim.ambient.movers) {
      const f = v.free;
      if (f && (f.px !== f.x || f.py !== f.y || f.pangle !== f.angle)) writeVehicle(v, true);
    }
    // … 3 lines
    for (const p of sim.pedViews) {
      // … 2 lines
      data[at + P.id] = p.id; data[at + P.x] = p.x; data[at + P.y] = p.y; data[at + P.heading] = p.heading;
      data[at + P.px] = p.prev.x; data[at + P.py] = p.prev.y; data[at + P.pheading] = p.prev.heading;
      data[at + P.v] = p.v; data[at + P.turnV] = p.turnV; data[at + P.age] = p.age;
      data[at + P.segment] = p.segment ?? -1; data[at + P.walking] = p.walking ? 1 : 0; data[at + P.kerbWait] = p.kerbWait; data[at + P.rank] = p.rank;
      // … 2 lines
      const was = this.pedSent.get(p.id);
      if (was && !pedDiffers(p, was.key)) { was.seen = frame; continue; }
      this.pedSent.set(p.id, { key: pedRichKey(p), seen: frame });
      peds.push([p.id, pedRich(p)]);
    }
    // … 9 lines
    this.post({
      // … 1 lines
      parked, meso, stats: this.stats(), wounds: takeWounds(sim),
      casualties: impactCasualties(sim, 0).filter((c) => !this.casualtiesSent.has(c) && (this.casualtiesSent.add(c), true)),
    }, transfer.length ? transfer : undefined);
  }

// src/sim/host/simHost.ts:332-352 slice of src/sim/host/simHost.ts#SimHost.parked (shared): the feature's statements; "…" marks code left out
  private parked(): ParkedChanges | null {
    // … 1 lines
    const changes = sim.ambient.parkedSince(this.parkedSerial, this);
    const serial = sim.ambient.parkedSerialNow();
    if (changes !== null && changes.length === 0) return null;
    this.parkedSerial = serial;
    const whole = changes === null;
    // … 1 lines
    const cars = whole ? sim.ambient.parked : [...new Set(changes.map((c) => c.car))];
    const out = new Float64Array(cars.length * PARKED_STRIDE);
    cars.forEach((car, k) => {
      const at = k * PARKED_STRIDE, f = car.free;
      out[at + PK.id] = car.id;
      out[at + PK.gone] = (whole || sim.ambient.isParked(car)) && f ? 0 : 1;
      out[at + PK.x] = f?.x ?? 0; out[at + PK.y] = f?.y ?? 0; out[at + PK.angle] = f?.angle ?? 0;
      out[at + PK.lot] = f?.lot ?? -1;
      out[at + PK.archetype] = this.archetypeIndex(car.archetype.id);
      out[at + PK.color] = parseInt(car.color.slice(1), 16);
    });
    return { whole, cars: out };
  }

// src/sim/host/simHost.ts:370-401 slice of src/sim/host/simHost.ts#SimHost.act (shared): the feature's statements; "…" marks code left out
  private act(act: SimAct): unknown {
    // … 2 lines
    switch (act.kind) {
      case 'shot': return sim.pedEngine.shot?.(sim, act.ped, act.part, act.fromX, act.fromY) ?? null;
      case 'impact': return sim.pedEngine.impact?.(sim, act.x, act.y, act.kill, act.scare) ?? 0;
      // … 10 lines
      case 'getUp': sim.pedEngine.getUp?.(sim, act.id, act.x, act.y, act.heading, act.seconds, act.crawl); return null;
    // … 13 lines
    }
  }

// src/sim/host/simHost.ts:444-444 slice of src/sim/host/simHost.ts#PED_RICH (shared): the feature's statements; "…" marks code left out
const PED_RICH = ['panic', 'maimed', 'lost', 'bleeding', 'wound', 'style', 'hand', 'carry', 'ageClass', 'gender', 'party', 'ground', 'stretch', 'waitingFor', 'gesture'] as const satisfies readonly (keyof PedView)[];

// src/sim/host/simHost.ts:446-448 slice of src/sim/host/simHost.ts#pedRichKey (shared): the feature's statements; "…" marks code left out
function pedRichKey(p: PedView): readonly unknown[] {
  return PED_RICH.map((field) => p[field]);
}

// src/sim/host/simHost.ts:451-454 slice of src/sim/host/simHost.ts#pedDiffers (shared): the feature's statements; "…" marks code left out
function pedDiffers(p: PedView, key: readonly unknown[]): boolean {
  for (let i = 0; i < PED_RICH.length; i++) if (p[PED_RICH[i]!] !== key[i]) return true;
// … 1 lines
}

// src/sim/host/simHost.ts:457-460 slice of src/sim/host/simHost.ts#pedRich (shared): the feature's statements; "…" marks code left out
function pedRich(p: PedView): PedRich {
  const { id: _id, x: _x, y: _y, heading: _h, prev: _p, v: _v, turnV: _t, age: _a, segment: _s, walking: _w, kerbWait: _k, rank: _r, ...rich } = p;
  return rich;
}
