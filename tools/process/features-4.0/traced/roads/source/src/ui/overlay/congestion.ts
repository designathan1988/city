// The code of src/ui/overlay/congestion.ts that is the feature's (the rest of the file is shared).

// src/ui/overlay/congestion.ts:25-36 slice of src/ui/overlay/congestion.ts#roadOccupancy (shared): the feature's statements; "…" marks code left out
export function roadOccupancy(net: Network, vehicles: Iterable<Moving>): Map<SegmentId, number> {
  // … 3 lines
  for (const [id, n] of count) {
    const ribbon = net.ribbons.get(id);
    if (!ribbon) continue;
    const lanes = Math.max(1, ribbon.road.lanes) * Math.max(1, ribbon.full.length);
    out.set(id, Math.min(1, (n * CAR_SPACE) / lanes));
  }
// … 1 lines
}

// src/ui/overlay/congestion.ts:39-39 slice of src/ui/overlay/congestion.ts#sampled (shared): the feature's statements; "…" marks code left out
const sampled = new WeakMap<SegmentRibbon, Float64Array>();

// src/ui/overlay/congestion.ts:42-58 slice of src/ui/overlay/congestion.ts#bandOf (shared): the feature's statements; "…" marks code left out
function bandOf(ribbon: SegmentRibbon): Float64Array {
  let band = sampled.get(ribbon);
  if (band) return band;
  const line = ribbon.full;
  const steps = Math.max(2, Math.min(48, Math.ceil(line.length / 12)));
  band = new Float64Array(4 + 2 * (steps + 1));
  const mid = line.sampleAt(line.length / 2);
  band[0] = mid.p.x; band[1] = mid.p.y;
  band[2] = mid.p.x + mid.n.x * ribbon.road.width; band[3] = mid.p.y + mid.n.y * ribbon.road.width;
  const frame = line.sampleAt(0);
  for (let k = 0; k <= steps; k++) {
    line.sampleAtInto((k / steps) * line.length, frame);
    band[4 + 2 * k] = frame.p.x; band[5 + 2 * k] = frame.p.y;
  }
  sampled.set(ribbon, band);
  return band;
}

// src/ui/overlay/congestion.ts:61-84 slice of src/ui/overlay/congestion.ts#drawCongestion (shared): the feature's statements; "…" marks code left out
export function drawCongestion(ctx: CanvasRenderingContext2D, net: Network, vehicles: Iterable<Moving>,
  view: readonly [number, number, number, number], at: (p: Vec2) => Vec2): void {
  const occupancy = roadOccupancy(net, vehicles);
  // … 5 lines
  for (const ribbon of net.ribbonsMeeting(view)) {
    const band = bandOf(ribbon);
    const a = atPoint(band, 0), b = atPoint(band, 2);
    const width = Math.max(3, Math.hypot(b.x - a.x, b.y - a.y) * 0.7);
    ctx.strokeStyle = congestionColour(occupancy.get(ribbon.id) ?? 0);
    ctx.lineWidth = width;
    ctx.globalAlpha = 0.75;
    ctx.beginPath();
    for (let i = 4; i < band.length; i += 2) {
      const p = atPoint(band, i);
      if (i === 4) ctx.moveTo(p.x, p.y); else ctx.lineTo(p.x, p.y);
    }
    ctx.stroke();
  }
// … 1 lines
}
