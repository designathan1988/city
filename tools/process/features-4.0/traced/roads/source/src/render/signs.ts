// The code of src/render/signs.ts that is the feature's (the rest of the file is shared).

// src/render/signs.ts:151-194 slice of src/render/signs.ts#streetChain (shared): the feature's statements; "…" marks code left out
export function streetChain(net: Network, at: Vec2): SegmentId[] {
  // … 3 lines
  for (const ribbon of net.ribbonsMeeting([at.x - reach, at.y - reach, at.x + reach, at.y + reach])) {
    const d = ribbon.full.distanceTo(at);
    if (d < best) { best = d; seed = ribbon.id; }
  }
  if (seed === null || best > reach) return [];
  const doc = net.doc;
  const chain = new Set<SegmentId>([seed]);
  // … 28 lines
  const s0 = doc.segment(seed)!;
  walk(seed, s0.a);
  walk(seed, s0.b);
  return [...chain];
}

// src/render/signs.ts:162-170 slice of src/render/signs.ts#streetChain.dirAt (shared): the feature's statements; "…" marks code left out
  const dirAt = (seg: SegmentId, node: NodeId): Vec2 | null => {
    const s = doc.segment(seg), r = net.ribbons.get(seg);
    if (!s || !r) return null;
    // … 1 lines
    const f = s.a === node ? r.full.sampleAt(Math.min(r.full.length, m(3))) : r.full.sampleAt(Math.max(0, r.full.length - m(3)));
    // … 1 lines
    const dx = f.p.x - n.x, dy = f.p.y - n.y, l = Math.hypot(dx, dy) || 1;
    return { x: dx / l, y: dy / l };
  };
