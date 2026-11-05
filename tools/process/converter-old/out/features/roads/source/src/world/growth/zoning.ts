// The code of src/world/growth/zoning.ts that is the feature's (the rest of the file is shared).

// src/world/growth/zoning.ts:128-133 slice of src/world/growth/zoning.ts#cornerOf.onStreet (shared): the feature's statements; "…" marks code left out
  const onStreet = (p: Vec2): boolean => net.ribbonsMeeting([p.x - STREET_MARGIN, p.y - STREET_MARGIN, p.x + STREET_MARGIN, p.y + STREET_MARGIN]).some((r) => {
    // … 1 lines
    const bb = r.full.bbox, reach = halfWidth(r.road, Level.Sidewalk) + STREET_MARGIN;
    if (p.x < bb.minX - reach || p.x > bb.maxX + reach || p.y < bb.minY - reach || p.y > bb.maxY + reach) return false;
    return r.full.distanceTo(p) < reach;
  });
