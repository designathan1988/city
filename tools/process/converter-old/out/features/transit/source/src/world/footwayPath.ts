// The code of src/world/footwayPath.ts that is the feature's (the rest of the file is shared).

// src/world/footwayPath.ts:16-19 interface FootwayNet: the feature's own
export interface FootwayNet {
  readonly doc: { segment(id: SegmentId): { readonly direction: SegmentDirection } | undefined };
  ribbonsMeeting(rect: ChangeRect): Iterable<FootwayRibbon>;
}

// src/world/footwayPath.ts:35-61 function nearestFootway: the feature's own
export function nearestFootway(net: FootwayNet, p: Vec2, reach: number, rejects?: (point: Vec2) => boolean): { x: number; y: number; segment: SegmentId; d: number } | null {
  let best: { x: number; y: number; segment: SegmentId; d: number } | null = null;
  for (const ribbon of net.ribbonsMeeting([p.x - reach, p.y - reach, p.x + reach, p.y + reach])) {
    const segment = net.doc.segment(ribbon.id);
    if (!segment) continue;
    for (const side of [1, -1] as const) {
      const path = footwayPath(ribbon, segment.direction, side);
      if (!path) continue;
      const hit = path.closestPoint(p);
      if (!(hit.distance < reach) || (best && hit.distance >= best.d)) continue;
      let point = hit.point, d = hit.distance;
      if (rejects?.(point)) {
        // The nearest point of this footway that is not turned down, a metre apart along it
        let found: { point: Vec2; d: number } | null = null;
        for (let s = 0, end = path.length; s <= end; s += REJECT_STEP) {
          const q = path.sampleAt(Math.min(s, end)).p;
          const dq = Math.hypot(q.x - p.x, q.y - p.y);
          if (dq < reach && (!found || dq < found.d) && !rejects(q)) found = { point: { x: q.x, y: q.y }, d: dq };
        }
        if (!found || (best && found.d >= best.d)) continue;
        ({ point, d } = found);
      }
      best = { x: point.x, y: point.y, segment: ribbon.id, d };
    }
  }
  return best;
}
