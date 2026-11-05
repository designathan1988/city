// The code of src/editor/barriers.ts that is the feature's (the rest of the file is shared).

// src/editor/barriers.ts:16-37 slice of src/editor/barriers.ts#snapBarrierPoint (shared): the feature's statements; "…" marks code left out
function snapBarrierPoint(net: Network | null, kind: BarrierKind, at: Vec2): Vec2 {
  if (!net) return at;
  // … 3 lines
  const reach = ROAD_CLEARANCE + CURB_BAND + half + m(0.15) + FOOTWAY_REACH;
  for (const ribbon of ribbonsNear(net, [at.x - reach, at.y - reach, at.x + reach, at.y + reach])) {
    if (net.doc.segment(ribbon.id)?.structure === 'tunnel') continue;
    // A kerb barrier stands just behind the kerb stone, on the footway's road edge; the others along the footway's back.
    const back = KERB_BARRIERS.has(kind)
      ? ribbon.road.width / 2 + CURB_BAND + half + m(0.15)
      : halfWidth(ribbon.road, Level.Sidewalk) + ROAD_CLEARANCE + half + m(0.02);
    const q = at;
    const hit = ribbon.full.closestPoint(q);
    const off = Math.abs(hit.distance - back);
    if (off > FOOTWAY_REACH || (best && off >= best.d)) continue;
    const frame = ribbon.full.sampleAt(hit.s);
    const side = (q.x - frame.p.x) * frame.n.x + (q.y - frame.p.y) * frame.n.y >= 0 ? 1 : -1;
    best = { d: off, p: { x: frame.p.x + frame.n.x * side * back, y: frame.p.y + frame.n.y * side * back } };
  }
  return best ? best.p : at;
}

// src/editor/barriers.ts:40-61 slice of src/editor/barriers.ts#barrierProblem (shared): the feature's statements; "…" marks code left out
function barrierProblem(net: Network | null, kind: BarrierKind, points: readonly Vec2[]): 'road' | 'short' | null {
  // … 6 lines
  if (!net) return null;
  // … 1 lines
  for (let i = 1; i < points.length; i++) {
    // … 8 lines
    if (KERB_BARRIERS.has(kind) ? onCarriageway(net, rect) : touchesRoad(net, rect)) return 'road';
  }
// … 1 lines
}

// src/editor/barriers.ts:64-73 slice of src/editor/barriers.ts#BarrierToolHost (shared): the feature's statements; "…" marks code left out
export interface BarrierToolHost {
  net(): Network | null;
// … 7 lines
}

// src/editor/barriers.ts:165-174 slice of src/editor/barriers.ts#onCarriageway (shared): the feature's statements; "…" marks code left out
function onCarriageway(net: Network, rect: readonly Vec2[]): boolean {
  // … 3 lines
  for (const ribbon of ribbonsNear(net, [x0, y0, x1, y1])) {
    const reach = ribbon.road.width / 2 + CURB_BAND - m(0.02);
    for (const p of rect) if (ribbon.full.closestPoint(p).distance < reach) return true;
  }
// … 1 lines
}
