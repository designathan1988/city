// The code of src/sim/agents/lotBays.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lotBays.ts:38-38 slice of src/sim/agents/lotBays.ts#BayLaneLine (shared): the feature's statements; "…" marks code left out
export type BayLaneLine = Pick<Lanelet, 'id' | 'kind' | 'length' | 'segment' | 'centre'>;

// src/sim/agents/lotBays.ts:48-57 slice of src/sim/agents/lotBays.ts#BayReads (shared): the feature's statements; "…" marks code left out
export interface BayReads {
  // … 3 lines
  ghost(lane: LaneletId): boolean;
// … 4 lines
}

// src/sim/agents/lotBays.ts:291-312 slice of src/sim/agents/lotBays.ts#gateLane (shared): the feature's statements; "…" marks code left out
function gateLane(r: BayReads, x: number, y: number, eitherSide = false): BayLane | null {
  // … 2 lines
  for (const lane of r.lanesNear(x, y, LANE_REACH)) {
    if (lane.kind !== 'link' || lane.length < m(10)) continue;
    if (r.ghost(lane.id)) continue;
    const hit = lane.centre.closestPoint({ x, y });
    if (hit.distance >= bestD) continue;
    const foot = lane.centre.sampleAt(hit.s);
    // … 1 lines
    if (!eitherSide && (x - foot.p.x) * foot.t.y - (y - foot.p.y) * foot.t.x <= 0) continue;
    if (eitherSide && r.bothWays(lane.segment!)) continue;
    const at = Math.max(GATE_LANE_END, Math.min(lane.length - GATE_LANE_END, hit.s + JOIN_PAST_GATE));
    const entryAt = Math.max(GATE_LANE_END, Math.min(lane.length - GATE_LANE_END, hit.s - STOP_BEFORE_GATE));
    // … 1 lines
    if (at - entryAt < m(3)) continue;
    const f = lane.centre.sampleAt(at);
    bestD = hit.distance;
    best = { lanelet: lane.id, at, x: f.p.x, y: f.p.y, tx: f.t.x, ty: f.t.y, entryAt };
  }
// … 1 lines
}

// src/sim/agents/lotBays.ts:340-366 slice of src/sim/agents/lotBays.ts#exitsOf (shared): the feature's statements; "…" marks code left out
function exitsOf(r: BayReads, b: LotRecord, v: { x: number; y: number; w: number; d: number }): LotExit[] {
  // … 9 lines
  const lanes = r.lanesNear(centre.x, centre.y, reach).filter((lane) => lane.centre.closestPoint(centre).distance < reach);
  // … 1 lines
  for (const s of sides) {
    // … 1 lines
    for (let t = EDGE_MARGIN; t <= s.length - EDGE_MARGIN; t += EXIT_STEP) {
      // … 1 lines
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

// src/sim/agents/lotBays.ts:369-389 slice of src/sim/agents/lotBays.ts#laneFor (shared): the feature's statements; "…" marks code left out
export function laneFor(r: BayReads, x: number, y: number, among?: Iterable<BayLaneLine>, segment?: SegmentId): BayLane | null {
  // … 5 lines
  for (const lane of listed ?? r.lanesNear(x, y, LANE_REACH)) {
    if (lane.kind !== 'link' || lane.length < 2 * LANE_END + m(6)) continue;
    if (segment !== undefined && lane.segment !== segment) continue;
    if (r.ghost(lane.id)) continue;
    const hit = lane.centre.closestPoint(q);
    if (hit.distance >= bestD) continue;
    const at = Math.max(LANE_END, Math.min(lane.length - LANE_END, hit.s));
    const f = lane.centre.sampleAt(at);
    // … 1 lines
    if ((q.x - f.p.x) * f.t.y - (q.y - f.p.y) * f.t.x <= 0) continue;
    bestD = hit.distance;
    best = { lanelet: lane.id, at, x: f.p.x, y: f.p.y, tx: f.t.x, ty: f.t.y };
  }
// … 1 lines
}

// src/sim/agents/lotBays.ts:394-400 slice of src/sim/agents/lotBays.ts#LotAsk (shared): the feature's statements; "…" marks code left out
export interface LotAsk {
  // … 2 lines
  readonly lanes: readonly { readonly lane: Lanelet; readonly ghost: boolean; readonly bothWays: boolean }[];
// … 2 lines
}

// src/sim/agents/lotBays.ts:406-422 slice of src/sim/agents/lotBays.ts#BayShare (shared): the feature's statements; "…" marks code left out
export interface BayShare {
  readonly lanes: readonly {
    readonly id: LaneletId; readonly kind: Lanelet['kind']; readonly length: number; readonly segment?: SegmentId;
    readonly ghost: boolean; readonly bothWays: boolean; readonly xy: Float64Array;
  }[];
// … 11 lines
}

// src/sim/agents/lotBays.ts:425-470 slice of src/sim/agents/lotBays.ts#shareOf (shared): the feature's statements; "…" marks code left out
export function shareOf(asks: readonly LotAsk[]): BayShare {
  const laneAt = new Map<Lanelet, number>(), wallAt = new Map<WallRing, number>(), kindAt = new Map<LotPart['kind'], number>();
  // … 3 lines
  const lots = asks.map((ask) => {
    // … 2 lines
    if (b.elements) {
      // … 2 lines
      b.elements.forEach((el, k) => {
        let kind = kindAt.get(el.kind);
        if (kind === undefined) { kindAt.set(el.kind, kind = kinds.length); kinds.push(el.kind); }
        partKinds![k] = kind;
      // … 3 lines
      });
    }
    return {
      // … 3 lines
      lanes: Int32Array.from(ask.lanes, ({ lane, ghost, bothWays }) => {
        let i = laneAt.get(lane);
        if (i === undefined) {
          laneAt.set(lane, i = lanes.length);
          lanes.push({ id: lane.id, kind: lane.kind, length: lane.length, ghost, bothWays, xy: lane.centre.xy, ...(lane.segment !== undefined ? { segment: lane.segment } : {}) });
        }
        return i;
      }),
      walls: Int32Array.from(ask.walls, (wall) => {
        let i = wallAt.get(wall);
        if (i === undefined) {
          wallAt.set(wall, i = walls.length);
          const ring = new Float64Array(wall.ring.length * 2);
          wall.ring.forEach((p, k) => { ring[2 * k] = p.x; ring[2 * k + 1] = p.y; });
          walls.push({ building: wall.building, ring, x0: wall.x0, y0: wall.y0, x1: wall.x1, y1: wall.y1 });
        }
        return i;
      }),
    };
  });
// … 1 lines
}

// src/sim/agents/lotBays.ts:489-519 slice of src/sim/agents/lotBays.ts#baysOfShare (shared): the feature's statements; "…" marks code left out
export function baysOfShare(share: BayShare, progress?: () => void): Bay[][] {
  // … 13 lines
  for (const l of lanes) if (l.segment !== undefined) segmentsBothWays.set(l.segment, l.bothWays);
  const ghosts = new Map(lanes.map((l) => [l.id, l.ghost]));
  return share.lots.map((lot) => {
    // … 2 lines
    const reads: BayReads = {
      lanesNear: (x, y, reach) => !Number.isFinite(x) || !Number.isFinite(y) ? own
        : own.filter((l) => cellsMeet(l.centre.bbox, x - reach, y - reach, x + reach, y + reach)),
      ghost: (id) => ghosts.get(id) ?? false,
    // … 2 lines
    };
  // … 3 lines
  });
}
