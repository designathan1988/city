// The code of src/sim/agents/lotDoors.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lotDoors.ts:12-20 slice of src/sim/agents/lotDoors.ts#DoorWay (shared): the feature's statements; "…" marks code left out
export interface DoorWay {
  readonly building: BuildingId;
// … 6 lines
}

// src/sim/agents/lotDoors.ts:48-95 slice of src/sim/agents/lotDoors.ts#workOut (shared): the feature's statements; "…" marks code left out
function workOut(b: TrafficBuilding): DoorWay | null {
  // … 2 lines
  const rings = b.footprints.map((ring) => ring.map((p) => worldToLocal(b, p)));
  if (!rings.length) return null;
  // … 2 lines
  for (const ring of rings) for (const p of ring) { cx += p.x; cy += p.y; n++; }
  cx /= n; cy /= n;
  for (const g of gates) {
    const alongY = g.facing === 1 || g.facing === 3;
    let ux = alongY ? 0 : 1, uy = alongY ? 1 : 0;
    if (g.angle) {
      const c = Math.cos(g.angle), s = Math.sin(g.angle);
      [ux, uy] = [ux * c - uy * s, ux * s + uy * c];
    }
    let nx = -uy, ny = ux;
    if ((cx - g.x) * nx + (cy - g.y) * ny < 0) { nx = -nx; ny = -ny; }
    // … 1 lines
    const t = hitRings(rings, g.x, g.y, nx, ny);
    if (t === null || t > DOOR_REACH || t < DOOR_OFF + m(0.5)) continue;
    const door = { x: g.x + nx * (t - DOOR_OFF), y: g.y + ny * (t - DOOR_OFF) };
    const inner = { x: g.x + nx * Math.min(GATE_IN, (t - DOOR_OFF) / 2), y: g.y + ny * Math.min(GATE_IN, (t - DOOR_OFF) / 2) };
    const outer = { x: g.x - nx * GATE_OUT, y: g.y - ny * GATE_OUT };
    // … 6 lines
    const grid = buildLotGrid(b, rect, rings, [{ inner, edge: outer }], PEOPLE_GRID);
    if (!sees(grid, inner, outer)) continue;
    const found = wayOut(grid, door.x, door.y);
    if (!found) continue;
    const local = [...found.points, inner, { x: g.x, y: g.y }, outer]
      .filter((p, i, all) => i === 0 || Math.hypot(p.x - all[i - 1]!.x, p.y - all[i - 1]!.y) > m(0.3));
    // … 2 lines
    for (const v of b.volumes) {
      for (const [x, y] of [[v.x, v.y], [v.x + v.w, v.y], [v.x, v.y + v.d], [v.x + v.w, v.y + v.d]] as const) {
        const a = (x - g.x) * ux + (y - g.y) * uy;
        lo = Math.min(lo, a); hi = Math.max(hi, a);
      }
    }
    return { building: b.id, out: local.map((p) => localToWorld(b, p.x, p.y)), frontage: Math.max(m(6), hi - lo) };
  }
// … 1 lines
}

// src/sim/agents/lotDoors.ts:98-109 slice of src/sim/agents/lotDoors.ts#streetWay (shared): the feature's statements; "…" marks code left out
function streetWay(b: TrafficBuilding): DoorWay | null {
  // … 2 lines
  const out = [
    { x: d.x + d.nx * DOOR_OFF, y: d.y + d.ny * DOOR_OFF },
    { x: d.x + d.nx * (DOOR_OFF + GATE_OUT), y: d.y + d.ny * (DOOR_OFF + GATE_OUT) },
  ];
  // … 2 lines
  for (const v of b.volumes) { lo = Math.min(lo, v.x); hi = Math.max(hi, v.x + v.w); }
  return { building: b.id, out, frontage: Math.max(m(6), hi - lo), street: true };
}
