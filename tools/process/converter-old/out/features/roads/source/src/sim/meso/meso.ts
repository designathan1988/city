// The code of src/sim/meso/meso.ts that is the feature's (the rest of the file is shared).

// src/sim/meso/meso.ts:64-64 slice of src/sim/meso/meso.ts#Meso.laneLists (shared): the feature's statements; "…" marks code left out
  private laneLists: (LaneletId[] | undefined)[] = [];

// src/sim/meso/meso.ts:569-569 slice of src/sim/meso/meso.ts#Meso.handoff (shared): the feature's statements; "…" marks code left out
  private handoff: LaneletId[] = [];

// src/sim/meso/meso.ts:576-592 slice of src/sim/meso/meso.ts#Meso.handoffLanes (shared): the feature's statements; "…" marks code left out
  handoffLanes(): readonly LaneletId[] {
    // … 11 lines
    const lanes: LaneletId[] = [];
    for (const o of [...slots].sort((a, b) => a - b)) lanes.push(...this.laneLists[o]!);
    this.handoff = lanes;
    return lanes;
  }

// src/sim/meso/meso.ts:599-609 slice of src/sim/meso/meso.ts#Meso.handoffDestination (shared): the feature's statements; "…" marks code left out
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
