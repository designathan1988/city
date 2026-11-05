// The code of src/sim/host/simClient.ts that is the feature's (the rest of the file is shared).

// src/sim/host/simClient.ts:28-52 slice of src/sim/host/simClient.ts#VehicleView (shared): the feature's statements; "…" marks code left out
export interface VehicleView {
  // … 6 lines
  peopleAge: readonly (PersonAgeClass | undefined)[];
// … 16 lines
}

// src/sim/host/simClient.ts:55-55 slice of src/sim/host/simClient.ts#PedRead (shared): the feature's statements; "…" marks code left out
type PedRead = PedView & { tickRead: number };

// src/sim/host/simClient.ts:129-129 slice of src/sim/host/simClient.ts#SimClient.pedViews (shared): the feature's statements; "…" marks code left out
  readonly pedViews: PedView[] = [];

// src/sim/host/simClient.ts:229-229 slice of src/sim/host/simClient.ts#SimClient.casualtyList (shared): the feature's statements; "…" marks code left out
  private casualtyList: Casualty[] = [];

// src/sim/host/simClient.ts:251-255 slice of src/sim/host/simClient.ts#SimClient.casualties (shared): the feature's statements; "…" marks code left out
  casualties(dt = 0): readonly Casualty[] {
    for (const c of this.casualtyList) c.t += dt;
    if (this.casualtyList.length && this.casualtyList[0]!.t > 60) this.casualtyList = this.casualtyList.filter((c) => c.t <= 60);
    return this.casualtyList;
  }

// src/sim/host/simClient.ts:548-707 slice of src/sim/host/simClient.ts#SimClient.readTick (shared): the feature's statements; "…" marks code left out
  private readTick(message: Extract<FromSim, { type: 'tick' }>): void {
    // … 100 lines
    for (let k = 0; k < m; k++) {
      // … 13 lines
      const w = view as { -readonly [K in keyof PedView]: PedView[K] };
      w.x = data[at + P.x]!; w.y = data[at + P.y]!; w.heading = data[at + P.heading]!;
      const prev = view.prev as { x: number; y: number; heading: number };
      prev.x = data[at + P.px]!; prev.y = data[at + P.py]!; prev.heading = data[at + P.pheading]!;
      w.v = data[at + P.v]!; w.turnV = data[at + P.turnV]!; w.age = data[at + P.age]!;
      // … 1 lines
      w.segment = segment < 0 ? undefined : segment as SegmentId;
      w.walking = data[at + P.walking] === 1; w.kerbWait = data[at + P.kerbWait]!; w.rank = data[at + P.rank]!;
      // … 1 lines
      const gesture = view.gesture;
      if (gesture) {
        let from = this.gestureFrom.get(gesture);
        if (from === undefined) this.gestureFrom.set(gesture, from = w.age - gesture.t);
        gesture.t = w.age - from;
      }
    // … 2 lines
    }
    // … 5 lines
    if (message.casualties.length) this.casualtyList.push(...message.casualties);
  // … 20 lines
  }
