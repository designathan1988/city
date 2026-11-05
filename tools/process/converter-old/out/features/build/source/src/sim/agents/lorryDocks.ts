// The code of src/sim/agents/lorryDocks.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lorryDocks.ts:57-60 slice of src/sim/agents/lorryDocks.ts#SOLID (shared): the feature's statements; "…" marks code left out
const SOLID: ReadonlySet<ElementKind> = new Set<ElementKind>([
  'stair', 'ramp', 'pillar', 'wall', 'fence', 'tree', 'bench', 'planter', 'railing', 'rocks', 'clock', 'hedge',
  'shrub', 'bin', 'lamp', 'bollard', 'parking',
]);

// src/sim/agents/lorryDocks.ts:76-87 slice of src/sim/agents/lorryDocks.ts#YardDock (shared): the feature's statements; "…" marks code left out
export interface YardDock {
  readonly dock: LorryDock;
// … 9 lines
}

// src/sim/agents/lorryDocks.ts:115-162 slice of src/sim/agents/lorryDocks.ts#Yard.constructor (shared): the feature's statements; "…" marks code left out
  constructor(readonly building: TrafficBuilding, public key: string, w: SimWorld, body: Steered, made: YardGrid) {
    // … 10 lines
    for (const exit of exits) {
      // … 4 lines
      const foot = lane.centre.closestPoint(localToWorld(b, exit.outer.x, exit.outer.y)).s;
      const stopAt = Math.max(m(1), foot - STOP_BEFORE), joinAt = Math.min(lane.length - m(1), foot + JOIN_PAST);
      const pose = (s: number): AxlePose => {
        // … 1 lines
        const p = worldToLocal(b, f.p), t = localDir(b, f.t.x, f.t.y);
        const angle = Math.atan2(t.y, t.x);
        return { x: p.x - Math.cos(angle) * body.rear, y: p.y - Math.sin(angle) * body.rear, angle };
      };
      this.gates.push({ exit, lane: exit.lane, stop: pose(stopAt), stopAt, join: pose(joinAt), joinAt });
    }
    for (const dock of b.docks) {
      const door = worldToLocal(b, dock.door), open = worldToLocal(b, dock.open);
      const ol = Math.hypot(open.x - door.x, open.y - door.y);
      const ox = (open.x - door.x) / ol, oy = (open.y - door.y) / ol;
      const angle = Math.atan2(oy, ox);
      // … 1 lines
      const docked = { x: door.x + ox * (centre - body.rear), y: door.y + oy * (centre - body.rear), angle };
      // … 3 lines
      for (let d = PRE_DOCK; d >= m(0.25); d -= m(0.25)) {
        const at = { x: docked.x + ox * d, y: docked.y + oy * d, angle };
      // … 1 lines
      }
      const order = this.gates.map((_, g) => g).sort((p, q) => gateDistance(this.gates[p]!, open) - gateDistance(this.gates[q]!, open));
      this.docks.push({
        dock, docked, pre: pre ?? docked, gate: -1, order,
      // … 2 lines
      });
    }
  }

// src/sim/agents/lorryDocks.ts:249-252 slice of src/sim/agents/lorryDocks.ts#Yard.toWorld (shared): the feature's statements; "…" marks code left out
  private toWorld(p: Waypoint): Waypoint {
    const q = localToWorld(this.building, p.x, p.y);
    return { x: q.x, y: q.y, angle: p.angle + this.building.rotation, reverse: p.reverse };
  }

// src/sim/agents/lorryDocks.ts:292-294 slice of src/sim/agents/lorryDocks.ts#lotsNear (shared): the feature's statements; "…" marks code left out
function lotsNear(w: SimWorld, x0: number, y0: number, x1: number, y1: number): Lot[] {
// … 1 lines
}

// src/sim/agents/lorryDocks.ts:297-305 slice of src/sim/agents/lorryDocks.ts#ownLot (shared): the feature's statements; "…" marks code left out
function ownLot(w: SimWorld, b: TrafficBuilding, lots: readonly Lot[]): Lot | undefined {
  let best: Lot | undefined, bestOrder = -Infinity;
  for (const lot of lots) {
    if (lot.building !== b.id) continue;
    const order = w.doc.lotStore.orderOf(lot.id) ?? -1;
    if (order >= bestOrder) { best = lot; bestOrder = order; }
  }
  return best;
}

// src/sim/agents/lorryDocks.ts:308-376 slice of src/sim/agents/lorryDocks.ts#buildYard (shared): the feature's statements; "…" marks code left out
export function* buildYard(b: TrafficBuilding, key: string, w: SimWorld, body: Steered): Generator<void, Yard, void> {
  // … 7 lines
  const ring = lot ? lot.corners.map((q) => worldToLocal(b, q)) : [];
  // … 3 lines
  if (ring.length) fillRing(solid, ring, 0);
  for (const { rings: world } of asphaltNear(w.net, wx0, wy0, wx1, wy1)) {
    fillRings(solid, world.map((r) => r.map(([x, y]) => worldToLocal(b, { x: x!, y: y! }))), 0);
  }
  // … 2 lines
  for (const exit of exits) {
    // … 5 lines
    const half = (gate?.w ?? m(5)) / 2 + MOUTH_FLARE, far = exit.out + m(2);
    fillRing(solid, [
      { x: gx - tx * half, y: gy - ty * half }, { x: gx + tx * half, y: gy + ty * half },
      { x: gx + tx * half + ox * far, y: gy + ty * half + oy * far }, { x: gx - tx * half + ox * far, y: gy - ty * half + oy * far },
    ], 0);
  }
  // … 2 lines
  if (ring.length) fillRing(inLot, ring, 1);
  // … 15 lines
  for (const other of lots) {
    // … 1 lines
    fillRing(solid, other.corners.map((q) => worldToLocal(b, q)), 1);
  }
  for (const o of w.buildings.near(wx0, wy0, wx1, wy1)) {
    for (const v of o.volumes) {
      if (v.open) continue;
      const vring = [localToWorld(o, v.x, v.y), localToWorld(o, v.x + v.w, v.y), localToWorld(o, v.x + v.w, v.y + v.d), localToWorld(o, v.x, v.y + v.d)]
        .map((q) => worldToLocal(b, q));
      if (vring.every((q) => q.x < box.x0 || q.x > box.x1) || vring.every((q) => q.y < box.y0 || q.y > box.y1)) continue;
      fillRing(solid, vring, 1);
    }
  }
  for (const el of b.elements ?? []) {
    if (!SOLID.has(el.kind) || el.z >= OVER_A_LORRY || isDock(el)) continue;
  // … 1 lines
  }
  // … 2 lines
  return new Yard(b, key, w, body, { inputs, reach, seen, ring, exits, solid, field });
}

// src/sim/agents/lorryDocks.ts:379-387 slice of src/sim/agents/lorryDocks.ts#yardBox (shared): the feature's statements; "…" marks code left out
function yardBox(b: TrafficBuilding, prop: { x: number; y: number; w: number; d: number }) {
  // … 3 lines
  const corners = [localToWorld(b, box.x0, box.y0), localToWorld(b, box.x1, box.y0), localToWorld(b, box.x1, box.y1), localToWorld(b, box.x0, box.y1)];
  const wx0 = Math.min(...corners.map((q) => q.x)), wx1 = Math.max(...corners.map((q) => q.x));
  const wy0 = Math.min(...corners.map((q) => q.y)), wy1 = Math.max(...corners.map((q) => q.y));
  return { x0, y0, nx, ny, box, wx0, wy0, wx1, wy1 };
}

// src/sim/agents/lorryDocks.ts:393-410 slice of src/sim/agents/lorryDocks.ts#yardInputs (shared): the feature's statements; "…" marks code left out
export function yardInputs(w: SimWorld, b: TrafficBuilding, lots?: readonly Lot[]): number {
  // … 10 lines
  for (const lot of lots ?? lotsNear(w, wx0, wy0, wx1, wy1)) for (const q of lot.corners) d.add(q.x).add(q.y);
  for (const o of w.buildings.near(wx0, wy0, wx1, wy1)) {
    // … 1 lines
    for (const v of o.volumes) d.add(v.x).add(v.y).add(v.w).add(v.d).add(v.open ? 1 : 0);
  }
// … 1 lines
}

// src/sim/agents/lorryDocks.ts:496-503 slice of src/sim/agents/lorryDocks.ts#nearestGate (shared): the feature's statements; "…" marks code left out
function nearestGate(gates: readonly TrafficPart[], x: number, y: number): TrafficPart | undefined {
  // … 1 lines
  for (const g of gates) {
    const d = Math.hypot(g.x - x, g.y - y);
    if (d < bestD) { bestD = d; best = g; }
  }
  return best;
}

// src/sim/agents/lorryDocks.ts:505-510 slice of src/sim/agents/lorryDocks.ts#elementRing (shared): the feature's statements; "…" marks code left out
function elementRing(el: LotPart): { x: number; y: number }[] {
  const alongY = el.facing === 0 || el.facing === 2;
  const hx = (alongY ? el.w : el.d) / 2, hy = (alongY ? el.d : el.w) / 2;
  const c = Math.cos(el.angle ?? 0), s = Math.sin(el.angle ?? 0);
  return [[-hx, -hy], [hx, -hy], [hx, hy], [-hx, hy]].map(([u, v]) => ({ x: el.x + u! * c - v! * s, y: el.y + u! * s + v! * c }));
}
