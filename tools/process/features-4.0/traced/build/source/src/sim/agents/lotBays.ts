// The code of src/sim/agents/lotBays.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lotBays.ts:26-26 slice of src/sim/agents/lotBays.ts#LotBlock (shared): the feature's statements; "…" marks code left out
type LotBlock = Pick<Volume, 'x' | 'y' | 'w' | 'd' | 'open' | 'base'>;

// src/sim/agents/lotBays.ts:28-35 slice of src/sim/agents/lotBays.ts#LotRecord (shared): the feature's statements; "…" marks code left out
export interface LotRecord {
  readonly id: BuildingId;
// … 5 lines
}

// src/sim/agents/lotBays.ts:41-45 slice of src/sim/agents/lotBays.ts#WallRing (shared): the feature's statements; "…" marks code left out
export interface WallRing {
  readonly building: BuildingId;
// … 2 lines
}

// src/sim/agents/lotBays.ts:77-108 slice of src/sim/agents/lotBays.ts#stallRoom (shared): the feature's statements; "…" marks code left out
function stallRoom(b: LotRecord, lx: number, ly: number, dx: number, dy: number, half: number): number {
  // … 16 lines
  for (const el of b.elements ?? []) {
    if (!STALL_BLOCKS[el.kind] || el.z >= OVER_A_BODY) continue;
    const alongY = el.facing === 0 || el.facing === 2;
    const hx = (alongY ? el.w : el.d) / 2, hy = (alongY ? el.d : el.w) / 2;
    if (Math.hypot(el.x - lx, el.y - ly) > reach + Math.hypot(hx, hy)) continue;
    const c = Math.cos(el.angle ?? 0), s = Math.sin(el.angle ?? 0);
    along([[-hx, -hy], [hx, -hy], [hx, hy], [-hx, hy]].map(([a, k]) => [el.x + a! * c - k! * s, el.y + a! * s + k! * c] as [number, number]));
  }
  for (const v of b.volumes) {
    if (v.open || v.base > 0) continue;
    if (Math.hypot(v.x + v.w / 2 - lx, v.y + v.d / 2 - ly) > reach + Math.hypot(v.w, v.d) / 2) continue;
    along([[v.x, v.y], [v.x + v.w, v.y], [v.x + v.w, v.y + v.d], [v.x, v.y + v.d]]);
  }
// … 1 lines
}

// src/sim/agents/lotBays.ts:134-206 slice of src/sim/agents/lotBays.ts#bayRows (shared): the feature's statements; "…" marks code left out
export function bayRows(r: BayReads, b: LotRecord): Bay[] {
  // … 4 lines
  for (const el of b.elements ?? []) {
    if (el.kind !== 'parking') continue;
    const stalls = Math.max(1, Math.floor(el.w / STALL));
    // … 1 lines
    if (el.w / stalls > LORRY_DOCK) continue;
    const alongY = el.facing === 0 || el.facing === 2;
    // … 1 lines
    let ax = alongY ? 0 : 1, ay = alongY ? 1 : 0;
    let cx = alongY ? 1 : 0, cy = alongY ? 0 : 1;
    if (el.angle) {
      const c = Math.cos(el.angle), s = Math.sin(el.angle);
      [ax, ay] = [ax * c - ay * s, ax * s + ay * c];
      [cx, cy] = [cx * c - cy * s, cx * s + cy * c];
    }
    const lot = property ?? b.volumes.find((v) => v.open && el.x >= v.x && el.x <= v.x + v.w && el.y >= v.y && el.y <= v.y + v.d);
    let nav = lot ? navCache.get(lot) : undefined;
    if (lot && !nav) {
      const exits = property ? gateExits(r, b, property) : exitsOf(r, b, lot);
      const local = wallsNear(r, b, lot).map((wl) => wl.ring.map((q) => worldToLocal(b, q)));
      nav = { exits, grid: buildLotGrid(b, lot, local, exits) };
      navCache.set(lot, nav);
    }
    for (let k = 0; k < stalls; k++) {
      const u = -el.w / 2 + (el.w * (k + 0.5)) / stalls;
      const lx = el.x + cx * u, ly = el.y + cy * u;
      const p = localToWorld(b, lx, ly);
      if (lot && nav) {
        // The stall opens on whichever side has an aisle that leads out of the lot
        const reach = el.d / 2 + m(2.8);
        let pick: { sign: number; points: { x: number; y: number }[]; exit: number; cost: number } | null = null;
        for (const sign of [1, -1]) {
          const aisle = { x: lx + ax * sign * reach, y: ly + ay * sign * reach };
          const way = wayOut(nav.grid, aisle.x, aisle.y);
          if (!way) continue;
          let cost = 0;
          for (let i = 1; i < way.points.length; i++) cost += Math.hypot(way.points[i]!.x - way.points[i - 1]!.x, way.points[i]!.y - way.points[i - 1]!.y);
          if (!pick || cost < pick.cost) pick = { sign, points: way.points, exit: way.exit, cost };
        }
        const exit = pick ? nav.exits[pick.exit] : undefined;
        const axis = localDirToWorld(b, ax * (pick?.sign ?? 1), ay * (pick?.sign ?? 1));
        const room = stallRoom(b, lx, ly, ax, ay, el.d / 2);
        if (!pick || !exit) {
          // No aisle from this stall leads out to a street: it is not used.
          out.push({ id: out.length, building: b.id, x: p.x, y: p.y, ox: axis.x, oy: axis.y, depth: el.d, lane: null, via: [], room, car: null });
          continue;
        }
        // Through a gate, the way runs on out of it to the footway's back.
        const local = [...pick.points, exit.inner, ...(exit.outer ? [exit.outer] : [])];
        const via = local.filter((q, i) => i === 0 || Math.hypot(q.x - local[i - 1]!.x, q.y - local[i - 1]!.y) > m(1))
          .map((q) => localToWorld(b, q.x, q.y));
        out.push({
          id: out.length, building: b.id, x: p.x, y: p.y, ox: axis.x, oy: axis.y,
          depth: el.d, lane: exit.lane, via, room, car: null,
        });
        continue;
      }
      const lane = laneFor(r, p.x, p.y);
      const axis = localDirToWorld(b, ax, ay);
      // Outside any lot: the stall opens towards the road it is reached from.
      const sign = lane && (lane.x - p.x) * axis.x + (lane.y - p.y) * axis.y < 0 ? -1 : 1;
      out.push({
        id: out.length, building: b.id, x: p.x, y: p.y, ox: axis.x * sign, oy: axis.y * sign,
        depth: el.d, lane, via: [], room: stallRoom(b, lx, ly, ax, ay, el.d / 2), car: null,
      });
    }
  }
// … 1 lines
}

// src/sim/agents/lotBays.ts:209-215 slice of src/sim/agents/lotBays.ts#wallsNear (shared): the feature's statements; "…" marks code left out
function wallsNear(r: BayReads, b: LotRecord, v: { x: number; y: number; w: number; d: number }): readonly WallRing[] {
  const corners = [localToWorld(b, v.x, v.y), localToWorld(b, v.x + v.w, v.y), localToWorld(b, v.x + v.w, v.y + v.d), localToWorld(b, v.x, v.y + v.d)];
  // … 1 lines
  const x0 = Math.min(...corners.map((c) => c.x)) - pad, x1 = Math.max(...corners.map((c) => c.x)) + pad;
  const y0 = Math.min(...corners.map((c) => c.y)) - pad, y1 = Math.max(...corners.map((c) => c.y)) + pad;
  return r.wallsIn(x0, y0, x1, y1);
}

// src/sim/agents/lotBays.ts:237-239 slice of src/sim/agents/lotBays.ts#carGates (shared): the feature's statements; "…" marks code left out
export function carGates<E extends LotPart = BuildingElement>(b: { readonly elements?: readonly E[] }): E[] {
  return (b.elements ?? []).filter((el) => el.kind === 'gate' && el.w >= CAR_GATE);
}

// src/sim/agents/lotBays.ts:242-245 slice of src/sim/agents/lotBays.ts#personGates (shared): the feature's statements; "…" marks code left out
export function personGates<E extends LotPart = BuildingElement>(b: { readonly elements?: readonly E[] }): E[] {
  // … 1 lines
  return (b.elements ?? []).filter((el) => el.kind === 'gate' && !cars.has(el));
}

// src/sim/agents/lotBays.ts:248-259 slice of src/sim/agents/lotBays.ts#gatedProperty (shared): the feature's statements; "…" marks code left out
export function gatedProperty(b: Pick<LotRecord, 'volumes' | 'elements'>): { x: number; y: number; w: number; d: number } | null {
  // … 2 lines
  for (const v of b.volumes) {
    if (!v.open) continue;
    x0 = Math.min(x0, v.x); y0 = Math.min(y0, v.y); x1 = Math.max(x1, v.x + v.w); y1 = Math.max(y1, v.y + v.d);
  }
  if (!Number.isFinite(x0)) return null;
  // … 2 lines
  return { x: x0 - pad, y: y0 - pad, w: x1 - x0 + 2 * pad, d: y1 - y0 + 2 * pad };
}

// src/sim/agents/lotBays.ts:262-288 slice of src/sim/agents/lotBays.ts#gateExits (shared): the feature's statements; "…" marks code left out
export function gateExits(r: BayReads, b: LotRecord, property: { x: number; y: number; w: number; d: number }): LotExit[] {
  // … 2 lines
  for (const g of carGates(b)) {
    const alongY = g.facing === 1 || g.facing === 3;
    let ux = alongY ? 0 : 1, uy = alongY ? 1 : 0;
    if (g.angle) {
      const c = Math.cos(g.angle), s = Math.sin(g.angle);
      [ux, uy] = [ux * c - uy * s, ux * s + uy * c];
    }
    // … 1 lines
    let nx = -uy, ny = ux;
    if ((cx - g.x) * nx + (cy - g.y) * ny < 0) { nx = -nx; ny = -ny; }
    const inner = { x: g.x + nx * EDGE_INSET, y: g.y + ny * EDGE_INSET };
    const edge = { x: g.x + nx * m(0.4), y: g.y + ny * m(0.4) };
    const outer = { x: g.x - nx * m(1.2), y: g.y - ny * m(1.2) };
    const wOuter = localToWorld(b, outer.x, outer.y);
    // … 1 lines
    const lane = gateLane(r, wOuter.x, wOuter.y) ?? gateLane(r, wOuter.x, wOuter.y, true);
    if (!lane) continue;
    const reach = Math.hypot(lane.x - wOuter.x, lane.y - wOuter.y);
    if (reach > EDGE_REACH + JOIN_PAST_GATE) continue;
    if (!clearOfWalls(r, localToWorld(b, g.x, g.y), wOuter, m(1.1), b.id)) continue;
    out.push({ inner, edge, outer, lane, out: reach });
  }
// … 1 lines
}

// src/sim/agents/lotBays.ts:320-337 slice of src/sim/agents/lotBays.ts#clearOfWalls (shared): the feature's statements; "…" marks code left out
function clearOfWalls(r: Pick<BayReads, 'wallsIn'>, a: { x: number; y: number }, b: { x: number; y: number },
  sweep = SWEEP, except: BuildingId | null = null): boolean {
  // … 3 lines
  const near = r.wallsIn(Math.min(a.x, b.x) - sweep, Math.min(a.y, b.y) - sweep, Math.max(a.x, b.x) + sweep, Math.max(a.y, b.y) + sweep)
    .filter((wl) => wl.building !== except);
  if (near.length === 0) return true;
  // … 1 lines
  for (let i = 0; i <= steps; i++) {
    // … 1 lines
    for (const side of [-sweep, 0, sweep]) {
      // … 1 lines
      if (near.some((wl) => p.x >= wl.x0 && p.x <= wl.x1 && p.y >= wl.y0 && p.y <= wl.y1 && pointInPolygon(p, wl.ring))) return false;
    }
  }
// … 1 lines
}

// src/sim/agents/lotBays.ts:340-366 slice of src/sim/agents/lotBays.ts#exitsOf (shared): the feature's statements; "…" marks code left out
function exitsOf(r: BayReads, b: LotRecord, v: { x: number; y: number; w: number; d: number }): LotExit[] {
  // … 7 lines
  const centre = localToWorld(b, v.x + v.w / 2, v.y + v.d / 2);
  // … 1 lines
  const lanes = r.lanesNear(centre.x, centre.y, reach).filter((lane) => lane.centre.closestPoint(centre).distance < reach);
  // … 1 lines
  for (const s of sides) {
    // … 1 lines
    for (let t = EDGE_MARGIN; t <= s.length - EDGE_MARGIN; t += EXIT_STEP) {
      const edge = localToWorld(b, s.ax + s.dx * t, s.ay + s.dy * t);
      const lane = laneFor(r, edge.x, edge.y, lanes);
      if (!lane) continue;
      const out1 = Math.hypot(lane.x - edge.x, lane.y - edge.y);
      if (out1 > EDGE_REACH) continue;
      // … 1 lines
      if (!clearOfWalls(r, localToWorld(b, inner.x, inner.y), lane)) continue;
      out.push({ inner, edge: { x: s.ax + s.dx * t - s.nx * m(0.2), y: s.ay + s.dy * t - s.ny * m(0.2) }, lane, out: out1 });
    }
  }
// … 1 lines
}

// src/sim/agents/lotBays.ts:406-422 slice of src/sim/agents/lotBays.ts#BayShare (shared): the feature's statements; "…" marks code left out
export interface BayShare {
  // … 4 lines
  readonly walls: readonly { readonly building: BuildingId; readonly ring: Float64Array; readonly x0: number; readonly y0: number; readonly x1: number; readonly y1: number }[];
  // … 1 lines
  readonly lots: readonly {
    readonly id: BuildingId; readonly x: number; readonly y: number; readonly rotation: number;
    readonly volumes: readonly LotBlock[];
    /** Null: the record has no parts at all (`elements` absent). */
    readonly parts: Float64Array | null;
    readonly partKinds: Uint8Array | null;
    readonly lanes: Int32Array;
    readonly walls: Int32Array;
  }[];
}

// src/sim/agents/lotBays.ts:425-470 slice of src/sim/agents/lotBays.ts#shareOf (shared): the feature's statements; "…" marks code left out
export function shareOf(asks: readonly LotAsk[]): BayShare {
  // … 4 lines
  const lots = asks.map((ask) => {
    // … 2 lines
    if (b.elements) {
      // … 2 lines
      b.elements.forEach((el, k) => {
        let kind = kindAt.get(el.kind);
        if (kind === undefined) { kindAt.set(el.kind, kind = kinds.length); kinds.push(el.kind); }
        partKinds![k] = kind;
        // … 1 lines
        parts![o] = el.x; parts![o + 1] = el.y; parts![o + 2] = el.w; parts![o + 3] = el.d;
        parts![o + 4] = el.facing; parts![o + 5] = el.z; parts![o + 6] = el.angle ?? 0; parts![o + 7] = el.angle !== undefined ? 1 : 0;
      });
    }
    return {
      // … 1 lines
      volumes: b.volumes.map((v) => ({ x: v.x, y: v.y, w: v.w, d: v.d, base: v.base, ...(v.open !== undefined ? { open: v.open } : {}) })),
    // … 19 lines
    };
  });
// … 1 lines
}
