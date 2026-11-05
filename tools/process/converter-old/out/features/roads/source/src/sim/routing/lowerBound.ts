// The code of src/sim/routing/lowerBound.ts that is the feature's (the rest of the file is shared).

// src/sim/routing/lowerBound.ts:6-6 slice of src/sim/routing/lowerBound.ts#FASTEST_LIMIT (shared): the feature's statements; "…" marks code left out
const FASTEST_LIMIT = kmh(ROAD_SECTION_LIMITS.speedKmh[1]);

// src/sim/routing/lowerBound.ts:9-11 slice of src/sim/routing/lowerBound.ts#LaneLookup (shared): the feature's statements; "…" marks code left out
export interface LaneLookup {
  lanelet(id: LaneletId): Lanelet | undefined;
}

// src/sim/routing/lowerBound.ts:30-33 slice of src/sim/routing/lowerBound.ts#goalEnd (shared): the feature's statements; "…" marks code left out
export function goalEnd(w: LaneLookup, goal: LaneletId): { readonly x: number; readonly y: number } | null {
  const target = w.lanelet(goal);
  return target ? target.centre.point(target.centre.n - 1) : null;
}

// src/sim/routing/lowerBound.ts:36-41 slice of src/sim/routing/lowerBound.ts#laneLowerBound (shared): the feature's statements; "…" marks code left out
export function laneLowerBound(w: LaneLookup, id: LaneletId, end: { readonly x: number; readonly y: number }): number {
  const lane = w.lanelet(id);
  if (!lane) return 0;
  const p = lane.centre.point(lane.centre.n - 1);
  return Math.hypot(end.x - p.x, end.y - p.y) / FASTEST_LIMIT;
}
