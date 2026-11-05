// The code of src/world/lots.ts that is the feature's (the rest of the file is shared).

// src/world/lots.ts:148-148 slice of src/world/lots.ts#planners (shared): the feature's statements; "…" marks code left out
const planners = new WeakMap<Network, LotPlanner>();

// src/world/lots.ts:151-155 slice of src/world/lots.ts#planLots (shared): the feature's statements; "…" marks code left out
export function planLots(doc: RoadDoc, net: Network): LotPlan {
  let planner = planners.get(net);
  if (!planner || planner.doc !== doc) planners.set(net, planner = new LotPlanner(doc, net));
  return planner.update();
}

// src/world/lots.ts:158-160 slice of src/world/lots.ts#forgetLotPlans (shared): the feature's statements; "…" marks code left out
export function forgetLotPlans(net: Network): void {
  planners.delete(net);
}

// src/world/lots.ts:209-210 slice of src/world/lots.ts#plannedStreet (shared): the feature's statements; "…" marks code left out
const plannedStreet = (doc: RoadDoc, r: SegmentRibbon): boolean =>
  doc.segment(r.id)?.structure === 'ground' && carriesPedestrians(r.road);

// src/world/lots.ts:252-279 slice of src/world/lots.ts#LandPaving.tile (shared): the feature's statements; "…" marks code left out
  private tile(ti: number, tj: number, key: number): Uint8Array | null {
    // … 6 lines
    for (const id of net.segmentCells.idsMeeting(grow([minX, minY, minX + TILE_SIDE, minY + TILE_SIDE], PAD))) {
      const ribbon = net.ribbons.get(id);
      if (!ribbon || !plannedStreet(doc, ribbon)) continue;
      const bb = ribbon.full.bbox;
      const i0 = Math.max(0, Math.floor((bb.minX - PAD - minX) / RASTER)), i1 = Math.min(TILE - 1, Math.floor((bb.maxX + PAD - minX) / RASTER));
      const j0 = Math.max(0, Math.floor((bb.minY - PAD - minY) / RASTER)), j1 = Math.min(TILE - 1, Math.floor((bb.maxY + PAD - minY) / RASTER));
      if (i0 > i1 || j0 > j1) continue;
      near = true;
      for (let j = j0; j <= j1; j++) label.fill(NEAR, j * TILE + i0, j * TILE + i1 + 1);
    }
    // … 2 lines
    if (near) {
      const steps = markPaved(doc, net, { minX, minY, W: TILE, H: TILE, step: RASTER }, label);
      while (!steps.next().done) { /* the whole tile at once */ }
      bits = new Uint8Array((TILE * TILE) >> 3);
      for (let k = 0; k < label.length; k++) if (label[k] === PAVED) bits[k >> 3]! |= 1 << (k & 7);
    }
    this.tiles.set(key, bits);
    return bits;
  }

// src/world/lots.ts:459-461 slice of src/world/lots.ts#LotPlanner.constructor (shared): the feature's statements; "…" marks code left out
  constructor(readonly doc: RoadDoc, private readonly net: Network) {
  // … 1 lines
  }

// src/world/lots.ts:464-472 slice of src/world/lots.ts#LotPlanner.update (shared): the feature's statements; "…" marks code left out
  update(): LotPlan {
    const delta = this.netState === null ? null : this.net.changesSince(this.netState);
    this.netState = this.net.state;
    // … 2 lines
    if (delta === null || entries === null || entries.some((e) => e.rects === null || (e.kind === 'lots' && !e.ids))) this.planAll();
    else if (delta.segments.size || delta.nodes.size || delta.rects.length || entries.length) this.planChanged(delta, entries);
  // … 1 lines
  }

// src/world/lots.ts:475-503 slice of src/world/lots.ts#LotPlanner.planAll (shared): the feature's statements; "…" marks code left out
  private planAll(): void {
    // … 14 lines
    for (const id of [...doc.segments.keys()].sort((a, b) => a - b)) {
      const ribbon = net.ribbons.get(id);
      if (!ribbon || !plannedStreet(doc, ribbon)) continue;
      // … 1 lines
      const bb = ribbon.full.bbox;
      forTiles([bb.minX - PAD, bb.minY - PAD, bb.maxX + PAD, bb.maxY + PAD], (key) => tiles.add(key));
    }
  // … 6 lines
  }

// src/world/lots.ts:506-566 slice of src/world/lots.ts#LotPlanner.planChanged (shared): the feature's statements; "…" marks code left out
  private planChanged(delta: NetworkDelta, entries: readonly Change[]): void {
    // … 3 lines
    const moved: ChangeRect[] = [...delta.rects];
    for (const id of delta.segments) { const r = net.segmentCells.rectOf(id); if (r) moved.push(r); }
    for (const id of delta.nodes) {
      const r = net.junctionCells.rectOf(id);
      if (r) moved.push(r);
      const node = doc.node(id);
      if (node) moved.push([node.x, node.y, node.x, node.y]);
    }
    // … 2 lines
    for (const r of moved) forTiles(grow(r, PAD + 2 * RASTER), (key) => tiles.add(key));
    // … 6 lines
    for (const r of moved) for (const lot of doc.lotsIn(r)) recheck.add(lot.id);
    // … 27 lines
    for (const id of net.segmentCells.idsMeetingAny(tileRects.map((r) => grow(r, STREET_GROW + SMALL_PIECE)))) put(streetWork(id));
    for (const id of delta.segments) put(streetWork(id));
  // … 9 lines
  }

// src/world/lots.ts:569-610 slice of src/world/lots.ts#LotPlanner.run (shared): the feature's statements; "…" marks code left out
  private run(list: PlanWork[], labels: LandLabels, paved: (p: Vec2) => boolean, follow: boolean): void {
    // … 17 lines
    for (let at = 0; at < list.length; at++) {
      // … 8 lines
      if (item.segment === null && !sameRect(old?.piece?.rect, now?.piece?.rect)) {
        for (const rect of [old?.piece?.rect, now?.piece?.rect]) {
          if (rect) for (const id of this.net.segmentCells.idsMeeting(grow(rect, STREET_GROW))) enqueue(streetWork(id), item.order);
        }
      }
    // … 8 lines
    }
  }

// src/world/lots.ts:779-854 slice of src/world/lots.ts#LotPlanner.streetPlan (shared): the feature's statements; "…" marks code left out
  private streetPlan(segId: SegmentId, order: PlanOrder, labels: LandLabels, paved: (p: Vec2) => boolean): Planned | null {
    // … 1 lines
    const ribbon = net.ribbons.get(segId);
    if (!ribbon || !plannedStreet(doc, ribbon)) return null;
    const line = ribbon.full;
    const face = halfWidth(ribbon.road, Level.Sidewalk) + m(0.3);
    const length = line.length;
    // … 6 lines
    for (const side of [1, -1] as const) {
      const point = (s: number, d: number): Vec2 => {
        const f = line.sampleAt(Math.max(0, Math.min(length, s)));
        return { x: f.p.x - f.t.y * side * d, y: f.p.y + f.t.x * side * d };
      };
      // … 4 lines
      for (let s = 0; s <= length; s += step) {
        const p = point(s, face + m(1.5)), q = point(s, face + LOT_DEPTH - m(1));
        const ok = open(p) && !paved(p) && !paved(q);
        if (ok && from < 0) from = s;
        if ((!ok || s + step > length) && from >= 0) { runs.push([from, ok ? s : s - step]); from = -1; }
      }
      for (const [s0, s1] of runs) {
        // … 2 lines
        const mid = point((s0 + s1) / 2, face);
        const key = `s:${Math.round(stepsIn(mid.x, 4))},${Math.round(stepsIn(mid.y, 4))},${Math.round(stepsIn(len, 4))}`;
        keys.push(key);
        // … 2 lines
        for (let k = 0, n = Math.max(1, Math.ceil(len / LOT_FRONTAGE)); k <= n; k++) {
          // … 1 lines
          edge.push(point(s, face - m(1)), point(s, face + LOT_DEPTH));
        }
        // … 1 lines
        if (doc.lotStore.hasKey(key)) continue;
        // … 4 lines
        for (const corners of this.around(stripBox, order, made, false, false)) {
          // … 1 lines
          for (const q of corners) {
            const c = line.closestPoint(q);
            const f = line.sampleAt(c.s);
            // … 1 lines
            const across = ((q.x - f.p.x) * -f.t.y + (q.y - f.p.y) * f.t.x) * side;
            if (across > face - m(1) && across < face + LOT_DEPTH + m(1)) near = true;
            ls0 = Math.min(ls0, c.s); ls1 = Math.max(ls1, c.s);
          }
          // … 1 lines
          if (near && ls1 > s0 + m(0.5) && ls0 < s1 - m(0.5)) taken.push([ls0, ls1]);
        }
        for (const [a, b] of cutSpans(freeSpans(s0, s1, taken, MIN_LOT), LOT_FRONTAGE)) {
          // … 1 lines
          const corners: [Vec2, Vec2, Vec2, Vec2] = side === 1
            ? [point(a, face - m(1)), point(b, face - m(1)), point(b, face + LOT_DEPTH), point(a, face + LOT_DEPTH)]
            : [point(b, face - m(1)), point(a, face - m(1)), point(a, face + LOT_DEPTH), point(b, face + LOT_DEPTH)];
          const c = lotCentre({ corners });
          // … 1 lines
          const land = landLot(net, corners);
          const fit = land && this.fitFree(land, order, made);
          if (fit) made.push({ key, corners: fit, box: rectAround(fit)! });
        }
      }
    }
    // … 1 lines
    const bb = line.bbox, r = face + LOT_DEPTH + m(5);
    return { reach: [bb.minX - r, bb.minY - r, bb.maxX + r, bb.maxY + r], piece: null, segment: segId, keys, keyRects, made };
  }

// src/world/lots.ts:861-868 slice of src/world/lots.ts#landLot (shared): the feature's statements; "…" marks code left out
function landLot(net: Network, corners: readonly Vec2[]): Vec2[] | null {
  const land = onLand(net, corners, false, LAND_CLEARANCE);
  if (!land) return null;
  if (Math.abs(lotArea({ corners: land })) < Math.abs(lotArea({ corners })) * 0.5) return null;
  // … 1 lines
  if (touchesRoad(net, offsetRing(land, -m(0.016)))) return null;
  return facingCorners(withFront(simplifyRing(land, m(0.01)), [corners[0]!, corners[1]!]));
}

// src/world/lots.ts:1044-1060 slice of src/world/lots.ts#pavingNear (shared): the feature's statements; "…" marks code left out
function pavingNear(net: Network, box: ChangeRect, grow = 0): MultiPoly | null {
  const ribbons = net.ribbonsMeeting(box);
  const junctions: [NodeId, ReadonlyMap<SurfaceLevel, Junction>][] = [];
  for (const id of net.junctionCells.idsMeeting(box)) {
    const byLevel = net.junctions.get(id);
    if (byLevel) junctions.push([id, byLevel]);
  }
  if (!ribbons.length && !junctions.length) return null;
  const paving = union(levelRingsOf(net, Level.Sidewalk, ribbons, junctions));
  if (grow === 0) return paving;
  return paving.map((raw) => raw.map((ring, k) => {
    const pts = ring.map(([x, y]) => ({ x: x!, y: y! }));
    // Grown all round: outer counter-clockwise, holes (the blocks) clockwise, so the right-hand offset grows the paving.
    const oriented = (k === 0) === (lotArea({ corners: pts }) > 0) ? pts : [...pts].reverse();
    return offsetRing(oriented, grow).map((p) => [p.x, p.y]);
  }));
}

// src/world/lots.ts:1069-1098 slice of src/world/lots.ts#cutLotsByRoads (shared): the feature's statements; "…" marks code left out
export function cutLotsByRoads(doc: RoadDoc, net: Network, rects: readonly ChangeRect[] | null): number {
  if (!net.ribbons.size || rects?.length === 0) return 0;
  // … 3 lines
  for (const lot of lots) {
    // … 4 lines
    const paving = pavingNear(net, [x0 - PAVING_MARGIN, y0 - PAVING_MARGIN, x1 + PAVING_MARGIN, y1 + PAVING_MARGIN]);
    if (!paving?.length) continue;
    // … 2 lines
    for (const poly of difference([[lot.corners.map((p) => [p.x, p.y])]], paving)) {
      const ring = (poly[0] ?? []).map(([x, y]) => ({ x: x!, y: y! }));
      if (ring.length < 3) continue;
      const area = Math.abs(lotArea({ corners: ring }));
      left += area;
      if (area >= MIN_LOT * MIN_LOT / 2) pieces.push(ring);
    }
    // … 1 lines
    if (Math.abs(lotArea(lot)) - left < CROSSED_AREA) continue;
  // … 4 lines
  }
// … 1 lines
}

// src/world/lots.ts:1101-1117 slice of src/world/lots.ts#onLand (shared): the feature's statements; "…" marks code left out
export function onLand(net: Network, points: readonly Vec2[], square = true, clearance = 0): Vec2[] | null {
  // … 5 lines
  const near = net.ribbons.size ? pavingNear(net, [x0 - margin, y0 - margin, x1 + margin, y1 + margin], clearance) ?? [] : null;
  if (!near) return [...points];
  // … 1 lines
  for (const poly of difference([[points.map((p) => [p.x, p.y])]], near)) {
    const ring = (poly[0] ?? []).map(([x, y]) => ({ x: x!, y: y! }));
    const area = Math.abs(lotArea({ corners: ring }));
    if (ring.length >= 3 && area > bestArea) { bestArea = area; best = ring; }
  }
  if (!best || bestArea < MIN_LOT * MIN_LOT / 2) return null;
  return square ? squareCorners(best) : best;
}

// src/world/lots.ts:1270-1279 slice of src/world/lots.ts#footwayEdgesNear (shared): the feature's statements; "…" marks code left out
function footwayEdgesNear(net: Network, box: ChangeRect): number[] {
  // … 1 lines
  for (const poly of pavingNear(net, box) ?? []) for (const ring of poly) {
    for (let k = 0; k < ring.length; k++) {
      const p = ring[k]!, q = ring[(k + 1) % ring.length]!;
      edges.push(p[0]!, p[1]!, q[0]!, q[1]!);
    }
  }
// … 1 lines
}

// src/world/lots.ts:1282-1328 slice of src/world/lots.ts#lotSnapper (shared): the feature's statements; "…" marks code left out
export function lotSnapper(doc: RoadDoc, net: Network, lots?: readonly Pick<Lot, 'id' | 'corners'>[], apart?: Vec2): (p: Vec2, reach: number, skip?: number) => { p: Vec2; kind: 'corner' | 'street' | 'lot' | null } {
  return (p, reach, skip) => {
    // … 17 lines
    const edges = footwayEdgesNear(net, [p.x - around, p.y - around, p.x + around, p.y + around]);
    for (let e = 0; e < edges.length / 4; e++) {
      const x0 = edges[e * 4]!, y0 = edges[e * 4 + 1]!, x1 = edges[e * 4 + 2]!, y1 = edges[e * 4 + 3]!;
      const dx = x1 - x0, dy = y1 - y0, len2 = dx * dx + dy * dy;
      const t = len2 > 0 ? Math.max(0, Math.min(1, ((p.x - x0) * dx + (p.y - y0) * dy) / len2)) : 0;
      const qx = x0 + dx * t, qy = y0 + dy * t, d = Math.hypot(qx - p.x, qy - p.y);
      if (d < bestD) { bestD = d; best = { x: qx, y: qy }; }
      if (len2 >= STRAIGHT_BACK * STRAIGHT_BACK && d < span) backs.push({ d, x0, y0, dx, dy });
    }
    // … 15 lines
    if (best) return { p: best, kind: 'street' };
  // … 1 lines
  };
}
