// The code of src/world/nearestRibbon.ts that is the feature's (the rest of the file is shared).

// src/world/nearestRibbon.ts:11-21 slice of src/world/nearestRibbon.ts#nearestRibbon (shared): the feature's statements; "…" marks code left out
export function nearestRibbon(net: Network, p: Vec2): { readonly ribbon: SegmentRibbon; readonly distance: number } | null {
  if (!net.ribbons.size || !Number.isFinite(p.x) || !Number.isFinite(p.y)) return null;
  for (let r = FIRST_REACH; ; r *= 4) {
    let best: { ribbon: SegmentRibbon; distance: number } | null = null;
    for (const ribbon of net.ribbonsMeeting([p.x - r, p.y - r, p.x + r, p.y + r])) {
      const distance = ribbon.full.distanceTo(p);
      if (!best || distance < best.distance) best = { ribbon, distance };
    }
    if ((best && best.distance <= r) || r >= LAST_REACH) return best;
  }
}
