// The code of src/world/zoneGrid.ts that is the feature's (the rest of the file is shared).

// src/world/zoneGrid.ts:15-15 slice of src/world/zoneGrid.ts#ZONE_CELL (shared): the feature's statements; "…" marks code left out
export const ZONE_CELL = GRID_CELL;

// src/world/zoneGrid.ts:54-54 slice of src/world/zoneGrid.ts#BUCKET (shared): the feature's statements; "…" marks code left out
const BUCKET = ZONE_CELL;

// src/world/zoneGrid.ts:87-103 slice of src/world/zoneGrid.ts#pavedTester (shared): the feature's statements; "…" marks code left out
export function pavedTester(doc: RoadDoc, net: Network): { onRoad: (p: Vec2) => boolean; onPlate: (p: Vec2) => boolean } {
// … 15 lines
}

// src/world/zoneGrid.ts:89-99 slice of src/world/zoneGrid.ts#pavedTester.onRoad (shared): the feature's statements; "…" marks code left out
  const onRoad = (p: Vec2): boolean => {
    for (const id of net.segmentCells.idsMeeting([p.x, p.y, p.x, p.y])) {
      const ribbon = net.ribbons.get(id);
      if (!ribbon || doc.segment(id)?.structure !== 'ground') continue;
      const reach = halfWidth(ribbon.road, Level.Sidewalk);
      const bb = ribbon.full.bbox;
      if (p.x < bb.minX - reach || p.x > bb.maxX + reach || p.y < bb.minY - reach || p.y > bb.maxY + reach) continue;
      if (ribbon.full.distanceTo(p) < reach - m(0.05)) return true;
    }
  // … 1 lines
  };

// src/world/zoneGrid.ts:108-112 slice of src/world/zoneGrid.ts#markPaved (shared): the feature's statements; "…" marks code left out
export function* markPaved(doc: RoadDoc, net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void> {
  yield* markRoads(doc, net, grid, label);
  yield* markPlates(net, grid, label);
// … 1 lines
}

// src/world/zoneGrid.ts:118-175 slice of src/world/zoneGrid.ts#markRoads (shared): the feature's statements; "…" marks code left out
function* markRoads(doc: RoadDoc, net: Network, grid: RasterGrid, label: Int32Array): Generator<void, void> {
  // … 1 lines
  for (const id of net.segmentCells.idsMeeting(gridRect(grid))) {
    const ribbon = net.ribbons.get(id);
    if (!ribbon || doc.segment(id)?.structure !== 'ground') continue;
    const filed = net.segmentCells.rectOf(id);
    if (!filed) continue;
    const reach = halfWidth(ribbon.road, Level.Sidewalk);
    const bb = ribbon.full.bbox;
    // The bounds `onRoad` tests, as it computes them.
    const bx0 = bb.minX - reach, bx1 = bb.maxX + reach, by0 = bb.minY - reach, by1 = bb.maxY + reach;
    const within = reach - m(0.05);
    if (!(within > 0)) continue;
    const r = within + SWEEP_SLACK;
    const xy = ribbon.full.xy;
    for (let k = 0; k + 1 < ribbon.full.n; k++) {
      const ax = xy[k * 2]!, ay = xy[k * 2 + 1]!, ex = xy[k * 2 + 2]!, ey = xy[k * 2 + 3]!;
      const j0 = Math.max(0, Math.floor((Math.min(ay, ey) - r - minY) / step - 0.5) - 1);
      const j1 = Math.min(H - 1, Math.ceil((Math.max(ay, ey) + r - minY) / step - 0.5) + 1);
      const len = Math.hypot(ex - ax, ey - ay);
      // The band's corners: the segment pushed either side by r.
      const nx = len > 0 ? (-(ey - ay) / len) * r : 0, ny = len > 0 ? ((ex - ax) / len) * r : 0;
      const qx = [ax + nx, ex + nx, ex - nx, ax - nx], qy = [ay + ny, ey + ny, ey - ny, ay - ny];
      for (let j = j0; j <= j1; j++) {
        const py = minY + (j + 0.5) * step;
        if (py < filed[1] || py > filed[3] || py < by0 || py > by1) continue;
        // The capsule's chord along this row: its two end discs and its band (convex, so their span).
        let xl = Infinity, xr = -Infinity;
        for (let e = 0; e < 2; e++) {
          const cx = e ? ex : ax, dy = py - (e ? ey : ay);
          if (Math.abs(dy) > r) continue;
          const h = Math.sqrt(r * r - dy * dy);
          xl = Math.min(xl, cx - h); xr = Math.max(xr, cx + h);
        }
        if (len > 0) {
          for (let e = 0; e < 4; e++) {
            const px0 = qx[e]!, py0 = qy[e]!, px1 = qx[(e + 1) % 4]!, py1 = qy[(e + 1) % 4]!;
            if (py < Math.min(py0, py1) || py > Math.max(py0, py1)) continue;
            if (py0 === py1) { xl = Math.min(xl, px0, px1); xr = Math.max(xr, px0, px1); continue; }
            const x = px0 + ((py - py0) * (px1 - px0)) / (py1 - py0);
            xl = Math.min(xl, x); xr = Math.max(xr, x);
          }
        }
        if (xl > xr) continue;
        const i0 = Math.max(0, Math.floor((xl - minX) / step - 0.5) - 1);
        const i1 = Math.min(W - 1, Math.ceil((xr - minX) / step - 0.5) + 1);
        const row = j * W;
        for (let i = i0; i <= i1; i++) {
          if (label[row + i] !== NEAR) continue;
          const px = minX + (i + 0.5) * step;
          if (px < filed[0] || px > filed[2] || px < bx0 || px > bx1) continue;
          if (Math.sqrt(segmentDistSq(xy, k, px, py)) < within) label[row + i] = PAVED;
        }
      }
    }
    yield;
  }
}
