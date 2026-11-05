// The code of src/world/walkways.ts that is the feature's (the rest of the file is shared).

// src/world/walkways.ts:312-315 slice of src/world/walkways.ts#Walkways.update (shared): the feature's statements; "…" marks code left out
  update(net: Network): WalkGraph {
    if (this.netState === net.state) return this.graph;
    return this.apply(net, this.plan(net, false), null);
  }

// src/world/walkways.ts:318-328 slice of src/world/walkways.ts#Walkways.updateSteps (shared): the feature's statements; "…" marks code left out
  *updateSteps(net: Network, measurer?: CornerMeasurer): Generator<void | typeof WAITING, WalkGraph> {
    if (this.netState === net.state) return this.graph;
    const from = this.netState, state = net.state;
    const plan = this.plan(net, measurer?.roundCorners !== undefined);
    if (!plan.asks.length) return this.apply(net, plan, null);
    const lines = yield* answered(measurer!.roundCorners!(plan.asks.map((a) => a.ask)), 'the corners are rounded');
    if (this.netState !== from || net.state !== state) return this.update(net);
    // … 1 lines
    plan.asks.forEach((a, k) => { const made = lines?.[k]; if (made) rounded.set(a.node, made); });
    return this.apply(net, plan, rounded);
  }

// src/world/walkways.ts:331-371 slice of src/world/walkways.ts#Walkways.plan (shared): the feature's statements; "…" marks code left out
  private plan(net: Network, ask: boolean): WalkPlan {
    const doc = net.doc;
    const delta = this.netState === null ? null : net.changesSince(this.netState);
    const { rawSegs, nodes } = delta === null ? this.everything(doc) : { rawSegs: new Set<SegmentId>(), nodes: new Set<NodeId>() };
    if (delta !== null) {
      // A node's record (its height: the deck of its roads) is read by every road at it.
      for (const id of delta.segments) rawSegs.add(id);
      for (const id of delta.nodes) {
        nodes.add(id);
        for (const seg of doc.node(id)?.incident ?? []) rawSegs.add(seg);
      }
      for (const rect of delta.rects) for (const id of this.reads.idsMeeting(rect)) nodes.add(id);
    }
    // … 1 lines
    for (const id of rawSegs) {
      const before = this.segments.get(id);
      if (before) { nodes.add(before.ends[0]); nodes.add(before.ends[1]); }
      const now = doc.segment(id);
      if (now) { nodes.add(now.a); nodes.add(now.b); }
    }
    // … 1 lines
    const cutSegs = new Set<SegmentId>(rawSegs);
    for (const id of nodes) for (const seg of doc.node(id)?.incident ?? []) cutSegs.add(seg);
    // … 1 lines
    for (const id of [...rawSegs].sort(ascending)) if (doc.segment(id)) shapes.set(id, footwayShapes(net, id));
    // … 1 lines
    if (ask) {
      for (const id of [...nodes].sort(ascending)) {
        const node = doc.node(id);
        if (!node) continue;
        const here = endsAt(doc, id, (seg) => rawSegs.has(seg) ? shapes.get(seg) ?? [] : this.segments.get(seg)?.raws ?? [], () => -1);
        const corners = cornersOf(net, id, here);
        const decks = new Set(corners.filter((c) => c.from.way.structure === c.to.way.structure).map((c) => c.from.way.structure));
        if (!decks.size) continue;
        const ringsOf = ringsIn(net, boxOf(node, here));
        const rings = [...decks].flatMap((deck) => [Level.Curb, Level.Sidewalk].map((level) => ({ level, deck, polys: flatRings(ringsOf(level, deck)) })));
        asks.push({ node: id, ask: { rings, corners: corners.map((c) => ({ from: endOf(c.from), to: endOf(c.to), avoid: avoiding(here, c) })) } });
      }
    }
    return { rawSegs, nodes, cutSegs, shapes, asks };
  }

// src/world/walkways.ts:374-407 slice of src/world/walkways.ts#Walkways.apply (shared): the feature's statements; "…" marks code left out
  private apply(net: Network, plan: WalkPlan, rounded: ReadonlyMap<NodeId, readonly (Float64Array | null)[]> | null): WalkGraph {
    const doc = net.doc;
    this.netState = net.state;
    // … 22 lines
    for (const [id, shapes] of plan.shapes) this.segments.set(id, this.footways(doc, id, shapes));
    for (const id of [...nodes].sort(ascending)) if (doc.node(id)) this.buildNode(net, id, rounded?.get(id) ?? null);
    for (const id of [...cutSegs].sort(ascending)) {
      // … 1 lines
      if (unit) this.cut(doc, id, unit);
    }
  // … 2 lines
  }

// src/world/walkways.ts:446-559 slice of src/world/walkways.ts#Walkways.buildNode (shared): the feature's statements; "…" marks code left out
  private buildNode(net: Network, nodeId: NodeId, rounded: readonly (Float64Array | null)[] | null): void {
    // … 1 lines
    const g = this.graph, doc = net.doc;
    const node = doc.requireNode(nodeId);
    // … 2 lines
    const here = endsAt(doc, nodeId, (seg) => this.segments.get(seg)?.raws ?? [], (raw, end) => (end ? raw.b : raw.a));
    const rect = boxOf(node, here);
    this.reads.file(nodeId, unit, rect);
    // … 23 lines
    const pairs = cornersOf(net, nodeId, here);
    const ready = rounded?.length === pairs.length ? rounded : null;
    pairs.forEach((c, k) => {
      const line = ready ? ready[k] : undefined;
      join(c.from, c.to, c.kerb, line === undefined ? roundCorner(endOf(c.from), endOf(c.to), avoiding(here, c), walking) : line && Polyline.fromFlat(line));
    });
    // … 28 lines
    if (node.incident.length >= 2) {
      for (const segId of node.incident) {
        const d = net.crosswalkDistanceAt(segId, nodeId);
        if (d <= 0) continue;
        const seg = doc.requireSegment(segId);
        const ribbon = net.ribbons.get(segId);
        if (!ribbon) continue;
        const section = sectionOf(ribbon.road, seg.direction);
        if (!section.walkable) continue;
        const f = orientedPolyline(doc, seg, nodeId).sampleAt(d);
        // Looking away from the node: from `a` its left is the road's left, from `b` its right.
        const fromA = seg.a === nodeId;
        const midLeft = bandMid((fromA ? section.left : section.right).through);
        const midRight = bandMid((fromA ? section.right : section.left).through);
        const left = { x: f.p.x + f.n.x * midLeft, y: f.p.y + f.n.y * midLeft };
        const right = { x: f.p.x - f.n.x * midRight, y: f.p.y - f.n.y * midRight };
        // Each end lands on the walkway nearest it on its own side - this road's footway or the corner beside it - cut there.
        const a = landOn(left, segId);
        const b = landOn(right, segId);
        if (a === null || b === null || a === b) continue;
        const pa = g.nodes[a]!, pb = g.nodes[b]!;
        // As wide as the zebra painted.
        others.push({ kind: 'crossing', path: Polyline.fromPoints([pa, pb]), width: CROSSWALK_DEPTH, lo: -CROSSWALK_DEPTH / 2, hi: CROSSWALK_DEPTH / 2, kerb: 0,
          segment: segId, node: nodeId, structure: deckOf(doc, segId), a, b });
      }
    }
    // … 2 lines
    const left = here.find((e) => e.side > 0), right = here.find((e) => e.side < 0);
    if (node.incident.length === 1 && left && right && left.way.structure === right.way.structure
      && Math.hypot(left.p.x - right.p.x, left.p.y - right.p.y) >= JOIN) {
      // A turning circle
      let line = node.end === 'bulb' ? bulbLine(doc, net.polylines, nodeId, Math.abs((left.p.x - node.x) * -left.into.y + (left.p.y - node.y) * left.into.x)) : null;
      if (line) {
        const first = line[0]!, last = line[line.length - 1]!;
        if (Math.hypot(first.x - left.p.x, first.y - left.p.y) > Math.hypot(last.x - left.p.x, last.y - left.p.y)) line = [...line].reverse();
        const inset = Math.min(left.inset, right.inset), outer = Math.min(left.outer, right.outer);
        const lo = left.kerbSide < 0 ? -inset : -outer, hi = left.kerbSide < 0 ? outer : inset;
        others.push({ kind: 'corner', path: Polyline.fromPoints([left.p, ...line, right.p]), width: hi - lo, lo, hi,
          kerb: left.kerbSide < 0 ? -1 : 1, node: nodeId, structure: left.way.structure, a: left.node, b: right.node });
      } else {
        others.push({ kind: 'crossing', path: Polyline.fromPoints([left.p, right.p]), width: m(2), lo: -m(1), hi: m(1), kerb: 0,
          segment: left.way.segment, node: nodeId, structure: left.way.structure, a: left.node, b: right.node, unmarked: true });
      }
    }
    for (const way of corners) unit.ways.push(g.add(way).id);
    for (const way of others) unit.ways.push(g.add(way).id);
  }

// src/world/walkways.ts:565-588 slice of src/world/walkways.ts#footwayShapes (shared): the feature's statements; "…" marks code left out
function footwayShapes(net: Network, segId: SegmentId): FootwayShape[] {
  const doc = net.doc;
  const seg = doc.requireSegment(segId);
  // … 1 lines
  const ribbon = net.ribbons.get(segId);
  if (!ribbon) return shapes;
  const section = sectionOf(ribbon.road, seg.direction);
  if (!section.walkable) return shapes;
  for (const side of [1, -1] as const) {
    // … 1 lines
    const path = footwayPath(ribbon, seg.direction, side);
    if (!path) continue;
    const zones = side > 0 ? section.left : section.right;
    const mid = bandMid(zones.through);
    const inset = mid - zones.curb.outer;
    const outer = zones.frontage.outer - mid;
    const first = path.sampleAt(0), last = path.sampleAt(path.length);
    // … 1 lines
    const lo = side > 0 ? -inset : -outer, hi = side > 0 ? outer : inset;
    shapes.push({ segment: segId, side, path, width: hi - lo, lo, hi, kerb: side > 0 ? -1 : 1, structure: deckOf(doc, segId),
      inset, outer, first: { p: first.p, t: first.t }, last: { p: last.p, t: last.t } });
  }
// … 1 lines
}

// src/world/walkways.ts:615-623 slice of src/world/walkways.ts#ringsIn (shared): the feature's statements; "…" marks code left out
function ringsIn(net: Network, rect: ChangeRect): RingsOf {
  return (level, deck) => {
    const junctions = net.junctionCells.idsMeeting(rect).flatMap((id) => {
      const at = net.junctions.get(id);
      return at ? [[id, at] as const] : [];
    });
    return levelRingsOf(net, level, net.ribbonsMeeting(rect), junctions, (id) => deckOf(net.doc, id) === deck);
  };
}

// src/world/walkways.ts:630-662 slice of src/world/walkways.ts#cornersOf (shared): the feature's statements; "…" marks code left out
function cornersOf(net: Network, nodeId: NodeId, here: readonly FootwayEnd[]): { readonly from: FootwayEnd; readonly to: FootwayEnd; readonly kerb: Vec2 | null }[] {
  // … 1 lines
  const node = net.doc.requireNode(nodeId);
  // … 1 lines
  const byLevel = net.junctions.get(nodeId);
  const junction = byLevel?.get(Level.Sidewalk);
  if (junction && junction.legs.length >= 2 && !junction.transition) {
    // Each leg's footway joins the next leg's round the node, in angular order, whatever the angle between them
    const pairs = junction.corners.length ? junction.corners
      : junction.legs.map((_, i) => ({ i, j: (i + 1) % junction.legs.length }));
    for (const corner of pairs) {
      const li = junction.legs[corner.i]!, lj = junction.legs[corner.j]!;
      // Leg i's LEFT boundary feeds the corner, leg j's RIGHT boundary leaves it.
      const from = sideOf(here, li.seg, node, li.nrm, 1);
      const to = sideOf(here, lj.seg, node, lj.nrm, -1);
      if (!from || !to || from === to) continue;
      const kerb = byLevel?.get(Level.Curb)?.corners.find((c) => c.i === corner.i && c.j === corner.j)?.fillet ?? null;
      out.push({ from, to, kerb: kerb?.c ?? null });
    }
  } else if (node.incident.length === 2) {
    // A node of two roads drawn as a taper or a bend, or not drawn at all
    const [s0, s1] = node.incident as [SegmentId, SegmentId];
    for (const e0 of here.filter((e) => e.way.segment === s0)) {
      let best: FootwayEnd | null = null, bestD = Infinity;
      for (const e1 of here.filter((e) => e.way.segment === s1)) {
        const d = Math.hypot(e1.p.x - e0.p.x, e1.p.y - e0.p.y);
        if (d < bestD) { best = e1; bestD = d; }
      }
      if (best) out.push({ from: e0, to: best, kerb: null });
    }
  }
// … 1 lines
}
