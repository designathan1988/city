// The code of src/editor/buildingSnap.ts that is the feature's (the rest of the file is shared).

// src/editor/buildingSnap.ts:40-64 slice of src/editor/buildingSnap.ts#snapPlacement (shared): the feature's statements; "…" marks code left out
export function snapPlacement(
  // … 1 lines
  net: Network | null,
// … 4 lines
): PlacementSnap {
  const road = net ? snapToRoad(net, size, cursor) : null;
  if (road) return flush(doc, road, size, false, ignore);
// … 14 lines
}

// src/editor/buildingSnap.ts:75-106 slice of src/editor/buildingSnap.ts#snapToRoad (shared): the feature's statements; "…" marks code left out
function snapToRoad(net: Network, size: FootprintSize, cursor: Vec2): PlacementSnap | null {
  // … 3 lines
  for (const ribbon of ribbonsNear(net, [at.x - reach, at.y - reach, at.x + reach, at.y + reach])) {
    const segment = net.doc.segment(ribbon.id);
    if (!segment || segment.structure === 'tunnel') continue;
    // The front on the back of the footway itself (see `ROAD_CLEARANCE`).
    const half = halfWidth(ribbon.road, Level.Sidewalk) + ROAD_CLEARANCE;
    const hit = ribbon.full.closestPoint(at);
    if (hit.distance > half + size.depth + ROAD_REACH) continue;
    if (best && hit.distance >= best.distance) continue;
    const frame = ribbon.full.sampleAt(hit.s);
    const side = (at.x - frame.p.x) * frame.n.x + (at.y - frame.p.y) * frame.n.y >= 0 ? 1 : -1;
    // Along the road, the anchor snaps to the grid from the road's start.
    const step = GRID;
    const s = Math.round(hit.s / step) * step;
    const along = ribbon.full.sampleAt(Math.max(0, Math.min(ribbon.full.length, s)));
    const ox = frame.n.x * side;
    const oy = frame.n.y * side;
    // The front (local -y) faces the road: (sin r, -cos r) = -outward.
    const rotation = Math.atan2(-ox, oy);
    best = {
      distance: hit.distance,
      anchor: { x: along.p.x + ox * half, y: along.p.y + oy * half },
      rotation,
      ribbon: ribbon.id,
    };
  }
  if (!best) return null;
  return { anchor: cornerFlush(net, best.anchor, best.rotation, size, best.ribbon, cursor), rotation: normaliseAngle(best.rotation), kind: 'road' };
}

// src/editor/buildingSnap.ts:109-145 slice of src/editor/buildingSnap.ts#cornerFlush (shared): the feature's statements; "…" marks code left out
function cornerFlush(net: Network, anchor: Vec2, rotation: number, size: FootprintSize, snapped: number, cursor: Vec2): Vec2 {
  // … 7 lines
  for (const ribbon of ribbonsNear(net, [midX - reach, midY - reach, midX + reach, midY + reach])) {
    if (ribbon.id === snapped) continue;
    const segment = net.doc.segment(ribbon.id);
    if (!segment || segment.structure === 'tunnel') continue;
    const back = halfWidth(ribbon.road, Level.Sidewalk) + ROAD_CLEARANCE;
    for (const side of [-1, 1]) {
      const px = anchor.x + ux * side * size.width / 2 + vx * size.depth / 2;
      const py = anchor.y + uy * side * size.width / 2 + vy * size.depth / 2;
      const hit = ribbon.full.closestPoint({ x: px, y: py });
      if (hit.distance > back + size.width + size.module) continue;
      const frame = ribbon.full.sampleAt(hit.s);
      // The road runs along this side (its tangent along the depth).
      if (Math.abs(frame.t.x * vx + frame.t.y * vy) < 0.9) continue;
      // And lies beyond it: how far the side is from that road's footway.
      const beyond = ((frame.p.x - px) * ux + (frame.p.y - py) * uy) * side;
      if (beyond <= 0) continue;
      const need = beyond - back;
      const candidateX = anchor.x + ux * need * side;
      const candidateY = anchor.y + uy * need * side;
      const along = (cursor.x - candidateX) * ux + (cursor.y - candidateY) * uy;
      const behind = (cursor.x - candidateX) * vx + (cursor.y - candidateY) * vy;
      const outsideAlong = Math.max(0, Math.abs(along) - size.width / 2);
      const outsideDepth = Math.max(0, -behind, behind - size.depth);
      if (Math.hypot(outsideAlong, outsideDepth) > size.module) continue;
      if (Math.abs(need) < Math.abs(bestNeed)) { bestNeed = need; shift = need * side; }
    }
  }
  return { x: anchor.x + ux * shift, y: anchor.y + uy * shift };
}
