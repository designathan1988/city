// The code of src/world/poleLines.ts that is the feature's (the rest of the file is shared).

// src/world/poleLines.ts:150-156 slice of src/world/poleLines.ts#polesInclude (shared): the feature's statements; "…" marks code left out
function polesInclude(net: Network): (id: SegmentId) => boolean {
  return (id) => {
    if (deckOf(net.doc, id) !== 'ground') return false;
    const ribbon = net.ribbons.get(id);
    return !!ribbon && carriesPedestrians(ribbon.road) && ribbon.road.sidewalk > 0;
  };
}

// src/world/poleLines.ts:187-190 slice of src/world/poleLines.ts#regionStamp (shared): the feature's statements; "…" marks code left out
function regionStamp(net: Network, rect: ChangeRect): string {
  // … 1 lines
  return `${net.segmentCells.stamp(around)}|${net.junctionCells.stamp(around)}`;
}

// src/world/poleLines.ts:201-209 slice of src/world/poleLines.ts#PoleLineState.linesIn (shared): the feature's statements; "…" marks code left out
  linesIn(net: Network, rect: ChangeRect): PoleLines {
    this.letGoStale(net);
    // … 2 lines
    if (known) { poleLinesWork.kept++; return known.lines; }
    const lines = poleLinesOf(net, rect);
    this.regions.set(key, { rect, stamp: regionStamp(net, rect), lines });
    return lines;
  }

// src/world/poleLines.ts:212-217 slice of src/world/poleLines.ts#PoleLineState.letGoStale (shared): the feature's statements; "…" marks code left out
  private letGoStale(net: Network): void {
    const epochs = `${net.segmentCells.epoch},${net.junctionCells.epoch}`;
    if (epochs === this.checkedAt) return;
    this.checkedAt = epochs;
    for (const [key, region] of this.regions) if (regionStamp(net, region.rect) !== region.stamp) this.regions.delete(key);
  }

// src/world/poleLines.ts:221-221 slice of src/world/poleLines.ts#stateOf (shared): the feature's statements; "…" marks code left out
const stateOf = (net: Network): PoleLineState => (net.poleLines ??= new PoleLineState());

// src/world/poleLines.ts:224-226 slice of src/world/poleLines.ts#poleLinesIn (shared): the feature's statements; "…" marks code left out
export function poleLinesIn(net: Network, rect: ChangeRect): PoleLines {
  return stateOf(net).linesIn(net, rect);
}

// src/world/poleLines.ts:230-257 slice of src/world/poleLines.ts#poleLinesOf (shared): the feature's statements; "…" marks code left out
export function poleLinesOf(net: Network, rect: ChangeRect): PoleLines {
  // … 1 lines
  const segments = net.segmentCells.idsMeeting(around), nodes = net.junctionCells.idsMeeting(around);
  const ribbons: SegmentRibbon[] = [];
  // … 1 lines
  for (const id of segments) {
    const ribbon = net.ribbons.get(id);
    if (ribbon) ribbons.push(ribbon);
    const segment = net.doc.segment(id);
    if (segment) { ends.add(segment.a); ends.add(segment.b); }
  }
  const junctions: [NodeId, ReadonlyMap<SurfaceLevel, Junction>][] = [];
  for (const id of nodes) {
    const byLevel = net.junctions.get(id);
    if (byLevel) junctions.push([id, byLevel]);
  }
  const include = polesInclude(net);
  const kerbed = union(levelRingsOf(net, Level.Curb, ribbons, junctions, include));
  const paving = union(levelRingsOf(net, Level.Sidewalk, ribbons, junctions, include));
  const contours = contoursOf(kerbed);
  const lines: PoleLines = {
    contours, paving, kerbed, edges: contourEdges(contours),
    accesses: crossingAccessesAt(net, [...ends].sort((a, b) => a - b)),
  };
  poleLinesWork.made++;
  poleLinesWork.segments += ribbons.length;
// … 1 lines
}

// src/world/poleLines.ts:260-262 slice of src/world/poleLines.ts#poleLinesAt (shared): the feature's statements; "…" marks code left out
export function poleLinesAt(net: Network, p: Vec2): PoleLines {
  return poleLinesIn(net, cellRect(cellAt(p.x, p.y)));
}

// src/world/poleLines.ts:318-338 slice of src/world/poleLines.ts#insideLevels (shared): the feature's statements; "…" marks code left out
function insideLevels(net: Network, p: Vec2): { readonly paving: boolean; readonly kerbed: boolean } {
  // … 7 lines
  const include = polesInclude(net);
  net.segmentCells.someAt(p.x, p.y, (id) => {
    const ribbon = net.ribbons.get(id);
    if (ribbon) add(ribbonRings(net, ribbon, include));
    return false;
  });
  net.junctionCells.someAt(p.x, p.y, (id) => {
    const byLevel = net.junctions.get(id);
    if (byLevel) add(junctionRings(net, id, byLevel, include));
    return false;
  });
// … 1 lines
}

// src/world/poleLines.ts:341-352 slice of src/world/poleLines.ts#ribbonRings (shared): the feature's statements; "…" marks code left out
function ribbonRings(net: Network, ribbon: SegmentRibbon, include: (id: SegmentId) => boolean): FootRings {
  const footRings = stateOf(net).rings;
  let rings = footRings.get(ribbon);
  if (!rings) {
    rings = {
      paving: windingRings(levelRingsOf(net, Level.Sidewalk, [ribbon], [], include)),
      kerbed: windingRings(levelRingsOf(net, Level.Curb, [ribbon], [], include)),
    };
    footRings.set(ribbon, rings);
  }
  return rings;
}

// src/world/poleLines.ts:355-366 slice of src/world/poleLines.ts#junctionRings (shared): the feature's statements; "…" marks code left out
function junctionRings(net: Network, id: NodeId, byLevel: ReadonlyMap<SurfaceLevel, Junction>, include: (id: SegmentId) => boolean): FootRings {
  const footRings = stateOf(net).rings;
  let rings = footRings.get(byLevel);
  if (!rings) {
    rings = {
      paving: windingRings(levelRingsOf(net, Level.Sidewalk, [], [[id, byLevel]], include)),
      kerbed: windingRings(levelRingsOf(net, Level.Curb, [], [[id, byLevel]], include)),
    };
    footRings.set(byLevel, rings);
  }
  return rings;
}

// src/world/poleLines.ts:369-381 slice of src/world/poleLines.ts#localRings (shared): the feature's statements; "…" marks code left out
function localRings(net: Network, rect: ChangeRect): FootRings[] {
  const include = polesInclude(net);
  // … 1 lines
  for (const id of net.segmentCells.idsMeeting(rect)) {
    const ribbon = net.ribbons.get(id);
    if (ribbon) out.push(ribbonRings(net, ribbon, include));
  }
  for (const id of net.junctionCells.idsMeeting(rect)) {
    const byLevel = net.junctions.get(id);
    if (byLevel) out.push(junctionRings(net, id, byLevel, include));
  }
// … 1 lines
}

// src/world/poleLines.ts:384-472 slice of src/world/poleLines.ts#markPlates (shared): the feature's statements; "…" marks code left out
export function* markPlates(net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void> {
  // … 2 lines
  const include = polesInclude(net);
  // … 17 lines
  for (const id of net.segmentCells.idsMeeting(rect)) {
    const ribbon = net.ribbons.get(id);
    if (ribbon) place(ribbonRings(net, ribbon, include), net.segmentCells.rectOf(id));
  }
  for (const id of net.junctionCells.idsMeeting(rect)) {
    const byLevel = net.junctions.get(id);
    if (byLevel) place(junctionRings(net, id, byLevel, include), net.junctionCells.rectOf(id));
  }
// … 59 lines
}

// src/world/poleLines.ts:475-478 slice of src/world/poleLines.ts#onFootway (shared): the feature's statements; "…" marks code left out
export function onFootway(net: Network, p: Vec2): boolean {
  const inside = insideLevels(net, p);
  return inside.paving && !inside.kerbed;
}

// src/world/poleLines.ts:481-484 slice of src/world/poleLines.ts#onPaving (shared): the feature's statements; "…" marks code left out
export function onPaving(net: Network, p: Vec2): boolean {
  const inside = insideLevels(net, p);
  return inside.paving || inside.kerbed;
}

// src/world/poleLines.ts:505-540 slice of src/world/poleLines.ts#kerbward (shared): the feature's statements; "…" marks code left out
export function kerbward(net: Network, p: Vec2, reach: number): Vec2 | null {
  // … 3 lines
  const rings = localRings(net, [p.x - bestD, p.y - bestD, p.x + bestD, p.y + bestD]);
  // … 2 lines
  for (const set of rings) for (const r of set.kerbed) if (x >= r.x0 && x <= r.x1 && y >= r.y0 && y <= r.y1) winding += windingOf(r, x, y);
  // … 1 lines
  if (winding !== 0) return kerbwardOfUnion(net, p, reach);
  // … 1 lines
  const reachSq = (bestD * CLIPPER_SCALE) ** 2;
  for (const set of rings) {
    for (const r of set.kerbed) {
      const bx = Math.max(r.x0 - x, 0, x - r.x1), by = Math.max(r.y0 - y, 0, y - r.y1);
      if (bx * bx + by * by > reachSq * (1 + 1e-9) + 1) continue;
      const xy = r.xy;
      kerbWork.edges += xy.length / 2;
      for (let i = 0; i < xy.length; i += 2) {
        const j = (i + 2) % xy.length;
        const ax = xy[i]! / CLIPPER_SCALE, ay = xy[i + 1]! / CLIPPER_SCALE;
        const dx = xy[j]! / CLIPPER_SCALE - ax, dy = xy[j + 1]! / CLIPPER_SCALE - ay;
        const len2 = dx * dx + dy * dy;
        const t = len2 > 0 ? Math.max(0, Math.min(1, ((p.x - ax) * dx + (p.y - ay) * dy) / len2)) : 0;
        const qx = ax + dx * t - p.x, qy = ay + dy * t - p.y;
        const d = Math.hypot(qx, qy);
        if (d <= 1e-9 || d > bestD + 1e-9) continue;
        const ux = qx / d, uy = qy / d;
        if (best && d >= bestD - 1e-9 && (ux < best.x || (ux === best.x && uy <= best.y))) continue;
        bestD = Math.min(bestD, d);
        best = { x: ux, y: uy };
      }
    }
  }
  return best;
}

// src/world/poleLines.ts:543-573 slice of src/world/poleLines.ts#kerbwardOfUnion (shared): the feature's statements; "…" marks code left out
function kerbwardOfUnion(net: Network, p: Vec2, reach: number): Vec2 | null {
  // … 3 lines
  const kerbed = poleLinesAt(net, p).kerbed;
  const boxes = ringBoxes(kerbed);
  // … 1 lines
  for (const poly of kerbed) {
    for (const ring of poly) {
      // A ring whose bounds are no nearer than the kerb found so far holds no nearer edge
      const box = boxes[r++]!;
      if (Math.hypot(Math.max(box[0] - p.x, 0, p.x - box[2]), Math.max(box[1] - p.y, 0, p.y - box[3])) > bestD + 1e-6) continue;
      kerbWork.edges += ring.length;
      for (let i = 0; i < ring.length; i++) {
        const a = ring[i]!, b = ring[(i + 1) % ring.length]!;
        const ax = a[0]!, ay = a[1]!, dx = b[0]! - ax, dy = b[1]! - ay;
        const len2 = dx * dx + dy * dy;
        const t = len2 > 0 ? Math.max(0, Math.min(1, ((p.x - ax) * dx + (p.y - ay) * dy) / len2)) : 0;
        const qx = ax + dx * t - p.x, qy = ay + dy * t - p.y;
        const d = Math.hypot(qx, qy);
        if (d <= 1e-9 || d > bestD + 1e-9) continue;
        // Two kerbs as near (a point on a street's middle line): the answer must not hang on the order the rings come in
        const x = qx / d, y = qy / d;
        if (best && d >= bestD - 1e-9 && (x < best.x || (x === best.x && y <= best.y))) continue;
        bestD = Math.min(bestD, d);
        best = { x, y };
      }
    }
  }
  return best;
}
