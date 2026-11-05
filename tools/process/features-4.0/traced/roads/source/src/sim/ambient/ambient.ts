// The code of src/sim/ambient/ambient.ts that is the feature's (the rest of the file is shared).

// src/sim/ambient/ambient.ts:52-60 slice of src/sim/ambient/ambient.ts#SceneryCar (shared): the feature's statements; "…" marks code left out
export interface SceneryCar extends StandingCar {
  // … 6 lines
  readonly lanelet: LaneletId;
}

// src/sim/ambient/ambient.ts:310-351 slice of src/sim/ambient/ambient.ts#AmbientWorld.roadEnds (shared): the feature's statements; "…" marks code left out
  private roadEnds(w: SimWorld): { node: number; x: number; y: number }[] {
    if (this.endsFor === w.net.state) return this.ends;
    // … 2 lines
    const delta = this.endsFor === null ? null : w.net.changesSince(this.endsFor);
    this.endsFor = w.net.state;
    const nodes = delta === null ? this.everyEnd(w) : new Set<number>();
    if (delta !== null) {
      for (const id of delta.nodes) nodes.add(id);
      for (const id of delta.segments) {
        const seg = w.doc.segment(id);
        if (seg) { nodes.add(seg.a); nodes.add(seg.b); }
      }
      // A walkway changed within reach of a road end's look (16 m round it) moves what it finds.
      for (const r of delta.rects) {
        for (const id of w.net.segmentCells.idsMeeting([r[0] - END_LOOK, r[1] - END_LOOK, r[2] + END_LOOK, r[3] + END_LOOK])) {
          const seg = w.doc.segment(id);
          if (seg) { nodes.add(seg.a); nodes.add(seg.b); }
        }
      }
      for (const id of nodes) this.dropEnds(id);
    }
    for (const id of [...nodes].sort((x, y) => x - y)) {
      const node = w.doc.node(id as never);
      if (!node || !w.doc.mapEdge(node.id)) continue;
      const found: { x: number; y: number }[] = [];
      // Round the end, out to past the widest footway: each side's walkway once.
      for (let k = 0; k < 16; k++) {
        const angle = (k / 16) * Math.PI * 2;
        for (const r of [m(4), m(8), m(12)]) {
          const q = engine.walkableNear.call(engine, w, node.x + Math.cos(angle) * r, node.y + Math.sin(angle) * r, m(3));
          if (!q || Math.hypot(q.x - node.x, q.y - node.y) > END_LOOK) continue;
          if (found.some((f) => Math.hypot(f.x - q.x, f.y - q.y) < m(3))) continue;
          found.push({ x: q.x, y: q.y });
        }
      }
      const slots: number[] = [];
      for (const q of found) { slots.push(this.ends.length); this.ends.push({ node: node.id, x: q.x, y: q.y }); }
      if (slots.length) this.endSlots.set(node.id, slots);
    }
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:651-695 slice of src/sim/ambient/ambient.ts#AmbientWorld.traffic (shared): the feature's statements; "…" marks code left out
  private traffic(w: SimWorld, focus: NonNullable<SimWorld['focus']>, reach: number, want: number): void {
    // … 28 lines
    for (let tries = 0, made = 0; tries < 48 && made < MAKE_PER_LOOK * (anywhere ? 4 : 1) && this.drivers.size < want; tries++) {
      // … 6 lines
      const s = arch.length + m(0.4) + w.rng.ambient.float() * Math.max(0, lane.length - arch.length - m(0.8));
      const at = lane.centre.sampleAt(s).p;
      const d = Math.hypot(at.x - focus.x, at.y - focus.y);
      const ok = anywhere ? d < reach : d < reach && !this.seen(w, focus, at.x, at.y, arch.length + m(4));
      if (!ok) continue;
      const id = spawnVehicleAt(w, lane.id, s, arch);
      if (id !== null) { this.drivers.add(id); made++; }
    }
  }

// src/sim/ambient/ambient.ts:698-706 slice of src/sim/ambient/ambient.ts#AmbientWorld.lanesAround (shared): the feature's statements; "…" marks code left out
  private lanesAround(w: SimWorld, focus: { x: number; y: number }, reach: number): { id: string; length: number; x: number; y: number }[] {
    // … 1 lines
    for (const lane of lanesInBox(w, focus.x - reach, focus.y - reach, focus.x + reach, focus.y + reach)) {
      if (lane.kind !== 'link' || lane.length < m(12)) continue;
      const mid = lane.centre.sampleAt(lane.length / 2).p;
      out.push({ id: lane.id, length: lane.length, x: mid.x, y: mid.y });
    }
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:709-712 slice of src/sim/ambient/ambient.ts#AmbientWorld.openingLanes (shared): the feature's statements; "…" marks code left out
  private openingLanes(w: SimWorld): { id: string; length: number; x: number; y: number }[] {
    if (w.topologyLog.state !== this.lanesFor) this.listLanes(w);
  // … 1 lines
  }

// src/sim/ambient/ambient.ts:715-726 slice of src/sim/ambient/ambient.ts#AmbientWorld.listLanes (shared): the feature's statements; "…" marks code left out
  private listLanes(w: SimWorld): void {
    this.lanesFor = w.topologyLog.state;
    // … 1 lines
    for (const lane of w.graph.lanelets.values()) {
      if (lane.kind !== 'link' || lane.length < m(12)) continue;
      const mid = lane.centre.sampleAt(lane.length / 2).p;
      this.lanes.push({ id: lane.id, length: lane.length, x: mid.x, y: mid.y });
    }
  // … 3 lines
  }
