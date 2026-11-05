// The code of src/world/parkingLayout.ts that is the feature's (the rest of the file is shared).

// src/world/parkingLayout.ts:62-67 slice of src/world/parkingLayout.ts#parkingLayout (shared): the feature's statements; "…" marks code left out
export function parkingLayout(net: Network, entrances: readonly [Vec2, Vec2][] | null = null): ParkingLayout {
  // … 2 lines
  for (const ribbon of net.ribbons.values()) parkingOf(net, ribbon, entrances, bays, lines);
// … 1 lines
}

// src/world/parkingLayout.ts:70-74 function parkingLinesOf: the feature's own
export function parkingLinesOf(net: Network, ribbon: SegmentRibbon, entrances: readonly [Vec2, Vec2][] | null = null): ParkingLine[] {
  const lines: ParkingLine[] = [];
  parkingOf(net, ribbon, entrances, [], lines);
  return lines;
}

// src/world/parkingLayout.ts:90-93 slice of src/world/parkingLayout.ts#parksAlong (shared): the feature's statements; "…" marks code left out
export function parksAlong(net: Network, ribbon: SegmentRibbon): boolean {
  const rt = ribbon.road;
  return (rt.parkingLeft > 0 || rt.parkingRight > 0) && net.doc.segment(ribbon.id)?.structure === 'ground';
}

// src/world/parkingLayout.ts:96-104 slice of src/world/parkingLayout.ts#parkingDigest (shared): the feature's statements; "…" marks code left out
export function parkingDigest(net: Network, ribbon: SegmentRibbon, digest: Digest): Digest {
  const segment = net.doc.segment(ribbon.id);
  if (!segment || !parksAlong(net, ribbon)) return digest.add(0);
  const rt = ribbon.road;
  return digest.add(1).add(rt.width).add(rt.parkingLeft).add(rt.parkingRight).addText(rt.parkingLeftKind).addText(rt.parkingRightKind)
    .addText(segment.direction).addAll(ribbon.full.xy)
    .add(endClear(net, ribbon.id, segment.a)).add(endClear(net, ribbon.id, segment.b))
    .add(net.mouthDistance(ribbon.id, segment.a)).add(net.mouthDistance(ribbon.id, segment.b));
}

// src/world/parkingLayout.ts:107-189 slice of src/world/parkingLayout.ts#parkingOf (shared): the feature's statements; "…" marks code left out
function parkingOf(net: Network, ribbon: SegmentRibbon, entrances: readonly [Vec2, Vec2][] | null,
  bays: ParkingBay[], lines: ParkingLine[]): void {
  const doc = net.doc;
  const rt = ribbon.road;
  if (!parksAlong(net, ribbon)) return;
  const segment = doc.segment(ribbon.id);
  if (!segment) return;
  const line = ribbon.full;
  const length = line.length;
  const half = rt.width / 2;
  const s0 = endClear(net, ribbon.id, segment.a);
  const s1 = length - endClear(net, ribbon.id, segment.b);
  const mouthA = net.mouthDistance(ribbon.id, segment.a);
  const mouthB = length - net.mouthDistance(ribbon.id, segment.b);
  // … 2 lines
  for (const side of [1, -1] as const) {
    const kind = side === 1 ? rt.parkingLeftKind : rt.parkingRightKind;
    const depth = side === 1 ? rt.parkingLeft : rt.parkingRight;
    if (kind === 'none' || depth <= 0) continue;
    if (kind === 'cycle') {
      // A cycle lane: red from mouth to mouth, a solid white line on the side of the traffic.
      const run = [Math.max(0, mouthA), Math.min(length, mouthB)] as const;
      if (run[1] - run[0] < m(4)) continue;
      const along = (offset: number): Vec2[] => {
        const pts: Vec2[] = [];
        const n = Math.max(2, Math.ceil(stepsIn(run[1] - run[0], 2)) + 1);
        for (let i = 0; i < n; i++) {
          const f = line.sampleAt(run[0] + ((run[1] - run[0]) * i) / (n - 1));
          pts.push({ x: f.p.x + f.n.x * offset * side, y: f.p.y + f.n.y * offset * side });
        }
        return simplifyLine(pts, STRAIGHT);
      };
      lines.push({ segment: ribbon.id, points: along(half - depth / 2), width: depth - m(0.1), color: CYCLE_RED, dash: null });
      lines.push({ segment: ribbon.id, points: along(half - depth + m(0.06)), width: m(0.12), color: WHITE, dash: null });
      continue;
    }
    // … 1 lines
    const travel = segment.direction === 'aToB' ? 1 : segment.direction === 'bToA' ? -1 : side === 1 ? -1 : 1;
    const blocked = frontagesOn(line, half, side, near ??= entrancesNear(doc, line, half));
    const runs = subtract([[s0, s1]], blocked);
    const pitch = PARKING_PITCH[kind];
    const at = (s: number, offset: number): Vec2 => {
      const f = line.sampleAt(Math.max(0, Math.min(length, s)));
      return { x: f.p.x + f.n.x * offset * side, y: f.p.y + f.n.y * offset * side };
    };
    const kerb = half, inner = half - depth;
    // … 4 lines
    for (const [lo, hi] of runs) {
      const count = Math.floor((hi - lo) / pitch + 1e-6);
      if (count < 1) continue;
      // Bays along the kerb, following the road's line
      const first = lo + (hi - lo - count * pitch) / 2;
      for (let k = 0; k < count; k++) {
        const a = first + k * pitch, b = a + pitch;
        const corners: [Vec2, Vec2, Vec2, Vec2] = [at(a, kerb), at(b, kerb), at(b, inner), at(a, inner)];
        const mid = line.sampleAt(Math.max(0, Math.min(length, (a + b) / 2)));
        const centre = { x: mid.p.x + mid.n.x * (kerb + inner) / 2 * side, y: mid.p.y + mid.n.y * (kerb + inner) / 2 * side };
        // A parked car faces the way the traffic beside it goes.
        bays.push({ segment: ribbon.id, side, kind, corners, centre, facing: { x: mid.t.x * travel, y: mid.t.y * travel } });
        // The division between this bay and the next, from the kerb across the lane.
        if (k > 0) lines.push({ segment: ribbon.id, points: [at(a, kerb), at(a, inner)], width: BAY_LINE, color: WHITE, dash: null });
      }
      const last = first + count * pitch;
      // The run closed across the parking lane at both ends.
      lines.push({ segment: ribbon.id, points: [at(first, kerb), at(first, inner)], width: BAY_LINE, color: WHITE, dash: null });
      lines.push({ segment: ribbon.id, points: [at(last, kerb), at(last, inner)], width: BAY_LINE, color: WHITE, dash: null });
      // The parking lane's edge, dashed along the whole run, following the road.
      lines.push({ segment: ribbon.id, points: runLine(first, last, inner), width: BAY_LINE, color: WHITE, dash: MER_DASH });
      parked.push([first, last]);
    }
    // … 2 lines
    const offset = kerb - m(0.25);
    for (const [lo, hi] of subtract([[mouthA, mouthB]], parked)) {
      if (hi - lo < m(0.5)) continue;
      lines.push({ segment: ribbon.id, points: runLine(lo, hi, offset), width: NO_PARKING_LINE, color: YELLOW, dash: null });
    }
  }
}

// src/world/parkingLayout.ts:200-209 slice of src/world/parkingLayout.ts#endClear (shared): the feature's statements; "…" marks code left out
function endClear(net: Network, seg: SegmentId, node: NodeId): number {
  const doc = net.doc;
  const degree = doc.degree(node);
  const mouth = net.mouthDistance(seg, node);
  if (degree <= 1) return m(1);
  const crossing = net.crosswalkDistanceAt(seg, node);
  const pastCrossing = crossing > 0 ? crossing + CROSSWALK_DEPTH / 2 + CROSSING_CLEAR : 0;
  if (degree === 2 && !doc.node(node)?.crossing) return Math.max(mouth, pastCrossing);
  return Math.max(mouth + CORNER_CLEAR, pastCrossing, net.stopLineDistance(seg, node) + CROSSING_CLEAR);
}
