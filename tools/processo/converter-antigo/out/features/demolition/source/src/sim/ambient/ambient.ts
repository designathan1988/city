// The code of src/sim/ambient/ambient.ts that is the feature's (the rest of the file is shared).

// src/sim/ambient/ambient.ts:120-153 slice of src/sim/ambient/ambient.ts#AmbientWorld.reset (shared): the feature's statements; "…" marks code left out
  reset(w: SimWorld): void {
    for (const id of this.walkers) removeWalker(w, id);
  // … 31 lines
  }

// src/sim/ambient/ambient.ts:155-195 slice of src/sim/ambient/ambient.ts#AmbientWorld.step (shared): the feature's statements; "…" marks code left out
  step(w: SimWorld): void {
    // … 2 lines
    for (const id of this.walkers) if (!walkerAlive(w, id)) this.walkers.delete(id);
  // … 36 lines
  }

// src/sim/ambient/ambient.ts:198-222 slice of src/sim/ambient/ambient.ts#AmbientWorld.trimToCeilings (shared): the feature's statements; "…" marks code left out
  private trimToCeilings(w: SimWorld): void {
    // … 2 lines
    for (const id of this.walkers) {
      // … 1 lines
      const at = walkerOf(w, id);
      if (!at || (focus && focus.detail && this.seen(w, focus, at.x, at.y, m(4)))) continue;
      removeWalker(w, id);
    // … 2 lines
    }
  // … 13 lines
  }

// src/sim/ambient/ambient.ts:250-289 slice of src/sim/ambient/ambient.ts#AmbientWorld.people (shared): the feature's statements; "…" marks code left out
  private people(w: SimWorld, focus: NonNullable<SimWorld['focus']>, reach: number, want: number): void {
    // … 3 lines
    for (const id of [...this.walkers]) {
      const at = walkerOf(w, id);
      if (!at) { this.walkers.delete(id); continue; }
      const d = Math.hypot(at.x - focus.x, at.y - focus.y);
      // … 1 lines
      const inSight = seen && this.seen(w, focus, at.x, at.y, m(4));
      if (!inSight && (d > far || this.walkers.size > want)) {
        removeWalker(w, id);
        this.walkers.delete(id);
      } else if (inSight) {
        // In sight on their last stretch: on to somewhere else, never ending where they are seen.
        const b = w.rng.ambient.float() * Math.PI * 2, e = reach * (0.3 + w.rng.ambient.float() * 0.7);
        walkOn(w, id, focus.x + Math.cos(b) * e, focus.y + Math.sin(b) * e, m(30));
      }
    }
  // … 20 lines
  }

// src/sim/ambient/ambient.ts:422-457 slice of src/sim/ambient/ambient.ts#AmbientWorld.walkFrom (shared): the feature's statements; "…" marks code left out
  private walkFrom(w: SimWorld, from: { node: number; x: number; y: number }, fromLot: DoorWay | null, toLot: DoorWay | null): number | null {
    // … 25 lines
    const id = fromLot || toLot ? walkThrough(w, trip, head, tail) : engine.walkTrip.call(engine, w, trip);
    if (id === null) return null;
    this.walkers.add(id);
    if (roams) this.roaming.add(id);
    // … 3 lines
    this.company(w, id, ageClass, from, to, roams, head, tail);
    return id;
  }

// src/sim/ambient/ambient.ts:471-490 slice of src/sim/ambient/ambient.ts#AmbientWorld.keepWalking (shared): the feature's statements; "…" marks code left out
  private keepWalking(w: SimWorld): void {
    // … 2 lines
    for (const id of [...this.roaming]) {
      // … 1 lines
      if (!onLastStretch(w, id)) continue;
      const at = walkerOf(w, id);
      if (!at) continue;
      // … 1 lines
      if (lot && walkOn(w, id, lot.out[0]!.x, lot.out[0]!.y, m(30), reversed(lot))) {
        this.roaming.delete(id);
        this.gateWalks.toGate++;
        continue;
      }
      // … 1 lines
      if (end && walkOn(w, id, end.x, end.y, m(30))) { this.roaming.delete(id); continue; }
      const p = this.across(w, at);
      if (p) walkOn(w, id, p.x, p.y, m(30));
    }
  }

// src/sim/ambient/ambient.ts:588-595 slice of src/sim/ambient/ambient.ts#AmbientWorld.pickPlace (shared): the feature's statements; "…" marks code left out
  private pickPlace(w: SimWorld, hour: number, out: boolean, not: DoorWay | null): DoorWay | null {
    // … 5 lines
    return this.places.pick(w.rng.ambient, coef, not, (p) => joinsFootway(w, p), WALK_DECAY, m(40));
  }

// src/sim/ambient/ambient.ts:598-614 slice of src/sim/ambient/ambient.ts#AmbientWorld.company (shared): the feature's statements; "…" marks code left out
  private company(w: SimWorld, leader: number, age: PersonAgeClass, from: { x: number; y: number }, to: { x: number; y: number }, roams: boolean,
    head: readonly { x: number; y: number }[] = NO_WAY, tail: readonly { x: number; y: number }[] = NO_WAY): void {
    // … 10 lines
    const id = head.length || tail.length ? walkThrough(w, trip, head, tail) : engine.walkTrip.call(engine, w, trip);
    if (id === null) return;
    this.walkers.add(id);
    if (roams) this.roaming.add(id);
  }
