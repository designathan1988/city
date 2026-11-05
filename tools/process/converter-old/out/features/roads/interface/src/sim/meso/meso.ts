// What the feature uses of src/sim/meso/meso.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/sim/meso/meso.ts:37-37 variable HANDOFF_TRIES
/** Handoff lanes tried for a new car before it goes without a destination (its lookahead drives it, the window's edge takes it back). */
const HANDOFF_TRIES = 4;

// src/sim/meso/meso.ts:64-64 field Meso.laneLists
private laneLists: (LaneletId[] | undefined)[] = [];

// src/sim/meso/meso.ts:569-569 field Meso.handoff
private handoff: LaneletId[] = [];

// src/sim/meso/meso.ts:576-592 method Meso.handoffLanes
/**
   * The lanes just outside the window that a road out of it leads onto: where a micro car of the meso is handed back (`takeBack`).
   * Found from the window's own links (those inside, the roads out of their end nodes), never from the map's: the cost follows the
   * window's size, not the city's.
   */
  handoffLanes(): readonly LaneletId[];

// src/sim/meso/meso.ts:599-609 field Meso.handoffDestination
/**
   * DO NOT send a car of the meso to a random exit of the whole map (`chooseVehicleDestination`): its trip in the micro simulation
   * ends where it leaves the window, so its goal is a lane just past the window's edge (`handoffLanes`) - its search bounded by the
   * window. Null: none it may reach (its lookahead drives it, and the edge takes it back all the same).
   */
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
