// The code of src/world/cityGen/plan.ts that is the feature's (the rest of the file is shared).

// src/world/cityGen/plan.ts:118-122 slice of src/world/cityGen/plan.ts#planCity.inside (shared): the feature's statements; "…" marks code left out
  const inside = (p: Vec2): boolean => {
    // … 1 lines
    const edge = R * (1 + 0.16 * valueNoise(Math.cos(a) * 2.2 + options.seed * 0.1, Math.sin(a) * 2.2));
    return Math.hypot(p.x, p.y) < edge && Math.abs(p.x) < ROUND_REACH - m(40) && Math.abs(p.y) < ROUND_REACH - m(40);
  };

// src/world/cityGen/plan.ts:137-154 slice of src/world/cityGen/plan.ts#planCity.zoneAt (shared): the feature's statements; "…" marks code left out
  const zoneAt = (p: Vec2, onMain: boolean): { use: ZoneUse; density: ZoneDensity } | 'park' => {
    // … 3 lines
    const parkNoise = valueNoise(p.x / m(140) + parkSalt % 97, p.y / m(140) - parkSalt % 89);
    if (options.parks > 0 && k > 0.15 && parkNoise > 1 - options.parks * 9) return 'park';
    // … 7 lines
    const heat = 1 - k + (options.density - 0.5) * 0.6 + valueNoise(p.x / m(220), p.y / m(220)) * 0.08;
    const density: ZoneDensity = heat > 0.72 ? 'high' : heat > 0.42 ? 'medium' : 'low';
    if (heat > 0.85 || (onMain && heat > 0.35)) return { use: 'commercial', density };
    return { use: 'residential', density };
  };
