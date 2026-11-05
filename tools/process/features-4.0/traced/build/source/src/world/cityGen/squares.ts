// The code of src/world/cityGen/squares.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/squares.ts:25-59 slice of src/world/cityGen/squares.ts#blocks (shared): the feature's statements; "…" marks code left out
function blocks(lots: readonly Lot[]): Lot[][] {
  const parent = lots.map((_, i) => i);
  // … 13 lines
  lots.forEach((l, i) => {
    const c = lotCentre(l), key = `${Math.floor(c.x / cell)},${Math.floor(c.y / cell)}`;
    let list = grid.get(key);
    if (!list) grid.set(key, list = []);
    list.push(i);
  });
  lots.forEach((l, i) => {
    const c = lotCentre(l);
    const cx = Math.floor(c.x / cell), cy = Math.floor(c.y / cell);
    for (let dx = -1; dx <= 1; dx++) for (let dy = -1; dy <= 1; dy++) {
      for (const j of grid.get(`${cx + dx},${cy + dy}`) ?? []) {
        if (j <= i || find(i) === find(j)) continue;
        if (l.corners.some((p) => near(p, lots[j]!)) || lots[j]!.corners.some((p) => near(p, l))) parent[find(i)] = find(j);
      }
    }
  });
  const out = new Map<number, Lot[]>();
  lots.forEach((l, i) => { const r = find(i); let list = out.get(r); if (!list) out.set(r, list = []); list.push(l); });
  return [...out.values()];
}

// src/world/cityGen/squares.ts:28-37 slice of src/world/cityGen/squares.ts#blocks.near (shared): the feature's statements; "…" marks code left out
  const near = (p: Vec2, l: Lot): boolean => {
    const q = l.corners;
    for (let i = 0; i < q.length; i++) {
      const a = q[i]!, b = q[(i + 1) % q.length]!;
      const ex = b.x - a.x, ey = b.y - a.y, l2 = ex * ex + ey * ey;
      const t = l2 > 0 ? Math.max(0, Math.min(1, ((p.x - a.x) * ex + (p.y - a.y) * ey) / l2)) : 0;
      if (Math.hypot(a.x + ex * t - p.x, a.y + ey * t - p.y) < m(1)) return true;
    }
    return false;
  };

// src/world/cityGen/squares.ts:100-152 slice of src/world/cityGen/squares.ts#planSquares (shared): the feature's statements; "…" marks code left out
export function planSquares(doc: RoadDoc, seed: number): SquarePlan[] {
  const lots = doc.lots as readonly Lot[];
  const byCell = new Map<string, { block: Lot[]; centre: Vec2; score: number }>();
  // … 1 lines
  for (const block of blocks(lots)) {
    const area = block.reduce((s, l) => s + Math.abs(lotArea(l)), 0) / (UNITS_PER_METER * UNITS_PER_METER);
    if (area < AREA_RANGE[0] || area > AREA_RANGE[1]) continue;
    let x = 0, y = 0, w = 0;
    for (const l of block) { const c = lotCentre(l), a = Math.abs(lotArea(l)); x += c.x * a; y += c.y * a; w += a; }
    const centre = { x: x / w, y: y / w };
    // On a main road: some lot of the block fronts one (its front within reach).
    const fronting = block.some((l) => onMainRoad({ x: (l.corners[0]!.x + l.corners[1]!.x) / 2, y: (l.corners[0]!.y + l.corners[1]!.y) / 2 }, m(20)));
    if (!fronting) continue;
    const kx = Math.floor(centre.x / NEIGHBOURHOOD), ky = Math.floor(centre.y / NEIGHBOURHOOD);
    const mid = { x: (kx + 0.5) * NEIGHBOURHOOD, y: (ky + 0.5) * NEIGHBOURHOOD };
    const score = Math.hypot(centre.x - mid.x, centre.y - mid.y);
    const key = `${kx},${ky}`;
    const known = byCell.get(key);
    if (!known || score < known.score) byCell.set(key, { block, centre, score });
  }
  // … 1 lines
  for (const { block, centre } of [...byCell.values()].sort((a, b) => a.centre.x - b.centre.x || a.centre.y - b.centre.y)) {
    const inside = (p: Vec2): boolean => block.some((l) => insideLot(p, l));
    // The block's own axes: the first lot's front.
    const f = block[0]!;
    const ux0 = f.corners[1]!.x - f.corners[0]!.x, uy0 = f.corners[1]!.y - f.corners[0]!.y, ul = Math.hypot(ux0, uy0) || 1;
    const u = { x: ux0 / ul, y: uy0 / ul }, v = { x: -u.y, y: u.x };
    const onWalk = (p: Vec2): boolean => {
      const dx = p.x - centre.x, dy = p.y - centre.y;
      return Math.abs(dx * u.x + dy * u.y) < m(2.2) || Math.abs(dx * v.x + dy * v.y) < m(2.2) || Math.hypot(dx, dy) < m(7);
    };
    const minX = Math.min(...block.flatMap((l) => l.corners.map((p) => p.x))), maxX = Math.max(...block.flatMap((l) => l.corners.map((p) => p.x)));
    const minY = Math.min(...block.flatMap((l) => l.corners.map((p) => p.y))), maxY = Math.max(...block.flatMap((l) => l.corners.map((p) => p.y)));
    const dabs: PaintDab[] = [];
    // The lawns are the ground's own grass; the walks are paved over it.
    for (let x = minX; x <= maxX; x += m(1.8)) for (let y = minY; y <= maxY; y += m(1.8)) {
      const p = { x, y };
      if (inside(p) && onWalk(p)) dabs.push({ kind: 'concrete', x, y, radius: m(1.4), strength: 1 });
    }
    // Trees: on a 9 m grid in the lawns, clear of the walks and of the block's edge.
    const random = rng(seed * 31 + out.length * 7919);
    const trees: PlantedTree[] = [];
    for (let a = -m(200); a <= m(200); a += m(9)) for (let b = -m(200); b <= m(200); b += m(9)) {
      const p = { x: centre.x + u.x * a + v.x * b, y: centre.y + u.y * a + v.y * b };
      if (!inside(p) || onWalk(p)) continue;
      const edgeClear = [[m(3), 0], [-m(3), 0], [0, m(3)], [0, -m(3)]].every(([da, db]) => inside({ x: p.x + u.x * da! + v.x * db!, y: p.y + u.y * da! + v.y * db! }));
      if (!edgeClear) continue;
      trees.push({ x: p.x, y: p.y, height: m(9 + random() * 5), yaw: random() * Math.PI * 2, seed: treeSeed('mixed', random) });
    }
    out.push({ lots: block.map((l) => l.id), centre, dabs, trees });
  }
// … 1 lines
}
