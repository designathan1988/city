// The code of src/sim/meso/meso.ts that is the feature's (the rest of the file is shared).

// src/sim/meso/meso.ts:50-50 field Meso.model: the feature's own
  model = new MesoModel(MESO_DT);

// src/sim/meso/meso.ts:64-64 field Meso.laneLists: the feature's own
  private laneLists: (LaneletId[] | undefined)[] = [];

// src/sim/meso/meso.ts:75-75 field Meso.outAt: the feature's own
  private outAt = new Threads();

// src/sim/meso/meso.ts:92-92 field Meso.inside: the feature's own
  private readonly inside = new Set<number>();

// src/sim/meso/meso.ts:566-566 field Meso.windowMoves: the feature's own
  private windowMoves = 0;

// src/sim/meso/meso.ts:567-567 field Meso.handoffAt: the feature's own
  private handoffAt = -1;

// src/sim/meso/meso.ts:568-568 field Meso.handoffNodes: the feature's own
  private handoffNodes = -1;

// src/sim/meso/meso.ts:569-569 field Meso.handoff: the feature's own
  private handoff: LaneletId[] = [];

// src/sim/meso/meso.ts:576-592 method Meso.handoffLanes: the feature's own
  handoffLanes(): readonly LaneletId[] {
    if (this.handoffAt === this.windowMoves && this.handoffNodes === this.nodesChanged) return this.handoff;
    this.handoffAt = this.windowMoves;
    this.handoffNodes = this.nodesChanged;
    const slots = new Set<number>();
    for (const s of this.inside) {
      if (!this.model.has(s)) continue;
      const outs = this.outAt;
      for (let o = outs.head(outs.group(this.model.toAt(s))); o >= 0; o = outs.after(o)) {
        if (this.model.has(o) && !this.model.isInside(o) && this.laneLists[o]?.length) slots.add(o);
      }
    }
    const lanes: LaneletId[] = [];
    for (const o of [...slots].sort((a, b) => a - b)) lanes.push(...this.laneLists[o]!);
    this.handoff = lanes;
    return lanes;
  }

// src/sim/meso/meso.ts:599-609 field Meso.handoffDestination: the feature's own
  readonly handoffDestination = (w: SimWorld, start: LaneletId, body: BodyClass): LaneletId | null => {
    const lanes = this.handoffLanes();
    if (!lanes.length) return null;
    const rng = w.rng.meso;
    for (let k = 0; k < HANDOFF_TRIES; k++) {
      const goal = lanes[rng.int(0, lanes.length - 1)]!;
      if (goal === start || !w.graph.lanelets.has(goal) || w.rt(goal).ghost) continue;
      if (w.laneReaches.mayReach(w, start, goal, body)) return goal;
    }
    return null;
  };

// src/sim/meso/meso.ts:728-728 field Meso.nodesChanged: the feature's own
  private nodesChanged = 1;
