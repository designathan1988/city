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
    const engine = w.pedEngine;
    if (!engine.walkTrip || !engine.walkableNear) return;
    // … 1 lines
    for (let made = 0; made < MAKE_PER_LOOK * (anywhere ? 4 : 1) && this.walkers.size < want; made++) {
      // … 2 lines
      const from = engine.walkableNear.call(engine, w, focus.x + Math.cos(a) * d, focus.y + Math.sin(a) * d, m(30));
      if (!from || (seen && !this.fresh && this.seen(w, focus, from.x, from.y, m(6)))) continue;
      // … 2 lines
      const to = engine.walkableNear.call(engine, w, focus.x + Math.cos(b) * e, focus.y + Math.sin(b) * e, m(30));
      if (!to || Math.hypot(to.x - from.x, to.y - from.y) < m(40)) continue;
      // … 1 lines
      const ageClass: PersonAgeClass = roll < 0.12 ? 'child' : roll < 0.28 ? 'elder' : 'adult';
      const id = engine.walkTrip.call(engine, w, {
        trip: this.nextTrip++, fromX: from.x, fromY: from.y, toX: to.x, toY: to.y,
        seed: Math.floor(w.rng.ambient.float() * 0x7fffffff), ageClass, reach: m(30),
      });
      if (id !== null) { this.walkers.add(id); this.company(w, id, ageClass, from, to, false); }
    }
  }

// src/sim/ambient/ambient.ts:310-351 slice of src/sim/ambient/ambient.ts#AmbientWorld.roadEnds (shared): the feature's statements; "…" marks code left out
  private roadEnds(w: SimWorld): { node: number; x: number; y: number }[] {
    // … 1 lines
    const engine = w.pedEngine;
    if (!engine.walkableNear) return this.ends;
    // … 18 lines
    for (const id of [...nodes].sort((x, y) => x - y)) {
      // … 4 lines
      for (let k = 0; k < 16; k++) {
        // … 1 lines
        for (const r of [m(4), m(8), m(12)]) {
          const q = engine.walkableNear.call(engine, w, node.x + Math.cos(angle) * r, node.y + Math.sin(angle) * r, m(3));
          if (!q || Math.hypot(q.x - node.x, q.y - node.y) > END_LOOK) continue;
          if (found.some((f) => Math.hypot(f.x - q.x, f.y - q.y) < m(3))) continue;
          found.push({ x: q.x, y: q.y });
        }
      }
    // … 3 lines
    }
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:367-419 slice of src/sim/ambient/ambient.ts#AmbientWorld.edgePeople (shared): the feature's statements; "…" marks code left out
  private edgePeople(w: SimWorld): void {
    const engine = w.pedEngine;
    if (!engine.walkTrip) return;
    // … 27 lines
    for (let made = 0, tries = 0; made < batch && tries < batch * 4; tries++) {
      // … 10 lines
      if (fromLot) {
      // … 2 lines
      } else {
        // … 3 lines
        if (engine.bridge.anyoneWithin(w, from.x, from.y, m(1.5), null)) continue;
      }
    // … 1 lines
    }
  }

// src/sim/ambient/ambient.ts:422-457 slice of src/sim/ambient/ambient.ts#AmbientWorld.walkFrom (shared): the feature's statements; "…" marks code left out
  private walkFrom(w: SimWorld, from: { node: number; x: number; y: number }, fromLot: DoorWay | null, toLot: DoorWay | null): number | null {
    const engine = w.pedEngine;
    if (!engine.walkTrip) return null;
    // … 16 lines
    const ageClass: PersonAgeClass = roll < 0.12 ? 'child' : roll < 0.28 ? 'elder' : 'adult';
    // … 2 lines
    const trip = {
      // … 1 lines
      seed: Math.floor(w.rng.ambient.float() * 0x7fffffff), ageClass, reach: m(30),
    };
    const id = fromLot || toLot ? walkThrough(w, trip, head, tail) : engine.walkTrip.call(engine, w, trip);
    if (id === null) return null;
    this.walkers.add(id);
    if (roams) this.roaming.add(id);
    // … 3 lines
    this.company(w, id, ageClass, from, to, roams, head, tail);
    return id;
  }

// src/sim/ambient/ambient.ts:460-468 slice of src/sim/ambient/ambient.ts#AmbientWorld.across (shared): the feature's statements; "…" marks code left out
  private across(w: SimWorld, from: { x: number; y: number }): { x: number; y: number } | null {
    const engine = w.pedEngine;
    for (let k = 0; k < 4; k++) {
      // … 1 lines
      const p = engine.walkableNear?.call(engine, w, from.x + Math.cos(a) * d, from.y + Math.sin(a) * d, m(30));
      if (p) return p;
    }
  // … 1 lines
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

// src/sim/ambient/ambient.ts:500-500 slice of src/sim/ambient/ambient.ts#AmbientWorld.foot (shared): the feature's statements; "…" marks code left out
  private foot: { way: Walkway; at: number }[] = [];

// src/sim/ambient/ambient.ts:536-574 slice of src/sim/ambient/ambient.ts#AmbientWorld.openPeople (shared): the feature's statements; "…" marks code left out
  private openPeople(w: SimWorld, n: number): void {
    const engine = w.pedEngine;
    if (!engine.walkTrip) return;
    // … 3 lines
    if (graph.state !== this.footFor) {
      this.footFor = graph.state;
      this.foot = [];
      this.footLength = 0;
      for (const way of graph.ways) {
        if (!way || way.kind !== 'footway' || way.path.length < m(4)) continue;
        this.foot.push({ way, at: this.footLength });
        this.footLength += way.path.length;
      }
    }
    // … 4 lines
    for (let made = 0, tries = 0; made < n && tries < n * 6; tries++) {
      const roll = rng.float() * this.footLength;
      // … 1 lines
      while (lo < hi) { const mid = (lo + hi + 1) >> 1; if (this.foot[mid]!.at <= roll) lo = mid; else hi = mid - 1; }
      const way = this.foot[lo]!.way;
      const f = way.path.sampleAt(Math.min(way.path.length, roll - this.foot[lo]!.at));
      // … 1 lines
      const across = way.lo + (way.hi - way.lo) * (0.3 + 0.4 * rng.float());
      const x = f.p.x - f.t.y * across, y = f.p.y + f.t.x * across;
      if (this.openSpots.some((q) => Math.hypot(q.x - x, q.y - y) < SPACE)) continue;
      if (engine.bridge.anyoneWithin(w, x, y, SPACE, null)) continue;
      // … 2 lines
      const id = this.walkFrom(w, { node: -1, x, y }, null, toLot);
      // … 1 lines
      this.openSpots.push({ x, y });
    // … 1 lines
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
    const engine = w.pedEngine;
    if (!engine.walkTrip) return;
    // … 2 lines
    const ageClass: PersonAgeClass = age === 'child' ? 'adult' : roll < 0.1 && age === 'adult' ? 'child' : age;
    // … 1 lines
    const trip = {
      // … 1 lines
      seed: Math.floor(w.rng.ambient.float() * 0x7fffffff), ageClass, reach: m(30), with: leader,
    };
    const id = head.length || tail.length ? walkThrough(w, trip, head, tail) : engine.walkTrip.call(engine, w, trip);
    if (id === null) return;
    this.walkers.add(id);
    if (roams) this.roaming.add(id);
  }
