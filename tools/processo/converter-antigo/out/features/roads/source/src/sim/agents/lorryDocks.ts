// The code of src/sim/agents/lorryDocks.ts that is the feature's (the rest of the file is shared).

// src/sim/agents/lorryDocks.ts:115-162 slice of src/sim/agents/lorryDocks.ts#Yard.constructor (shared): the feature's statements; "…" marks code left out
  constructor(readonly building: TrafficBuilding, public key: string, w: SimWorld, body: Steered, made: YardGrid) {
    // … 10 lines
    for (const exit of exits) {
      // … 4 lines
      const foot = lane.centre.closestPoint(localToWorld(b, exit.outer.x, exit.outer.y)).s;
      const stopAt = Math.max(m(1), foot - STOP_BEFORE), joinAt = Math.min(lane.length - m(1), foot + JOIN_PAST);
      const pose = (s: number): AxlePose => {
        const f = lane.centre.sampleAt(s);
        const p = worldToLocal(b, f.p), t = localDir(b, f.t.x, f.t.y);
        const angle = Math.atan2(t.y, t.x);
        return { x: p.x - Math.cos(angle) * body.rear, y: p.y - Math.sin(angle) * body.rear, angle };
      };
      this.gates.push({ exit, lane: exit.lane, stop: pose(stopAt), stopAt, join: pose(joinAt), joinAt });
    }
  // … 21 lines
  }

// src/sim/agents/lorryDocks.ts:393-410 slice of src/sim/agents/lorryDocks.ts#yardInputs (shared): the feature's statements; "…" marks code left out
export function yardInputs(w: SimWorld, b: TrafficBuilding, lots?: readonly Lot[]): number {
  // … 4 lines
  for (const lane of lanesInBox(w, wx0 - GATE_LANES_READ, wy0 - GATE_LANES_READ, wx1 + GATE_LANES_READ, wy1 + GATE_LANES_READ)) {
    d.addText(lane.id).add(lane.length).addAll(lane.centre.xy);
  }
// … 9 lines
}

// src/sim/agents/lorryDocks.ts:422-422 slice of src/sim/agents/lorryDocks.ts#yardKey (shared): the feature's statements; "…" marks code left out
export const yardKey = (w: SimWorld): string => `${w.topologyRevision}:${w.graph.revision}:${w.net.revision}`;

// src/sim/agents/lorryDocks.ts:475-489 slice of src/sim/agents/lorryDocks.ts#asphaltNear (shared): the feature's statements; "…" marks code left out
function asphaltNear(net: Network, x0: number, y0: number, x1: number, y1: number): AsphaltPiece[] {
  // … 1 lines
  const junctions = net.junctionCells.idsMeeting(box).flatMap((id) => {
    const at = net.junctions.get(id);
    return at ? [[id, at] as const] : [];
  });
  // … 1 lines
  for (const rings of levelRingsOf(net, Level.Asphalt as never, net.ribbonsMeeting(box), junctions)) {
    let px0 = Infinity, py0 = Infinity, px1 = -Infinity, py1 = -Infinity;
    for (const [x, y] of rings[0] ?? []) { px0 = Math.min(px0, x!); px1 = Math.max(px1, x!); py0 = Math.min(py0, y!); py1 = Math.max(py1, y!); }
    if (px1 < x0 || px0 > x1 || py1 < y0 || py0 > y1) continue;
    out.push({ rings, box: { x0: px0, y0: py0, x1: px1, y1: py1 } });
  }
// … 1 lines
}
