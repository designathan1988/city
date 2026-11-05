// The code of src/world/cityGen/plan.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/plan.ts:61-69 slice of src/world/cityGen/plan.ts#CityPlan (shared): the feature's statements; "…" marks code left out
export interface CityPlan {
  // … 6 lines
  zoneAt(p: Vec2, onMain: boolean): { use: ZoneUse; density: ZoneDensity } | 'park';
}

// src/world/cityGen/plan.ts:137-154 slice of src/world/cityGen/plan.ts#planCity.zoneAt (shared): the feature's statements; "…" marks code left out
  const zoneAt = (p: Vec2, onMain: boolean): { use: ZoneUse; density: ZoneDensity } | 'park' => {
    const near = Math.min(...centres.map((c, i) => Math.hypot(p.x - c.x, p.y - c.y) / (i === 0 ? 1 : 0.75)));
    const k = near / R; // 0 at a centre, ~1 at the edge
    // Parks: whole patches, by a coarse noise, never in the very centre.
    const parkNoise = valueNoise(p.x / m(140) + parkSalt % 97, p.y / m(140) - parkSalt % 89);
    if (options.parks > 0 && k > 0.15 && parkNoise > 1 - options.parks * 9) return 'park';
    // Industry: a sector of the edge, away from the centres.
    if (options.industry && k > 0.62) {
      let d = Math.abs(Math.atan2(p.y, p.x) - industryAngle) % (Math.PI * 2);
      if (d > Math.PI) d = Math.PI * 2 - d;
      if (d < 0.55) return { use: 'industrial', density: k > 0.85 ? 'high' : 'medium' };
    }
    // The bid-rent curve, shifted by how dense a city was asked for.
    const heat = 1 - k + (options.density - 0.5) * 0.6 + valueNoise(p.x / m(220), p.y / m(220)) * 0.08;
    const density: ZoneDensity = heat > 0.72 ? 'high' : heat > 0.42 ? 'medium' : 'low';
    if (heat > 0.85 || (onMain && heat > 0.35)) return { use: 'commercial', density };
    return { use: 'residential', density };
  };
