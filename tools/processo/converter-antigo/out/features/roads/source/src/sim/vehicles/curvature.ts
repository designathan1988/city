// The code of src/sim/vehicles/curvature.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/curvature.ts:23-23 slice of src/sim/vehicles/curvature.ts#profiles (shared): the feature's statements; "…" marks code left out
const profiles = new WeakMap<Lanelet, Float32Array>();

// src/sim/vehicles/curvature.ts:25-25 slice of src/sim/vehicles/curvature.ts#peaks (shared): the feature's statements; "…" marks code left out
const peaks = new WeakMap<Lanelet, number>();

// src/sim/vehicles/curvature.ts:30-49 slice of src/sim/vehicles/curvature.ts#profileOf (shared): the feature's statements; "…" marks code left out
function profileOf(lane: Lanelet): Float32Array {
  const cached = profiles.get(lane);
  if (cached) return cached;
  const n = Math.max(1, Math.ceil(lane.length / SAMPLE) + 1);
  const out = new Float32Array(n);
  const h = Math.min(CHORD, lane.length / 2);
  // … 1 lines
  if (h > 1e-3) {
    for (let i = 0; i < n; i++) {
      const s = Math.min(Math.max(i * SAMPLE, h), lane.length - h);
      const a = lane.centre.sampleAt(s - h).t;
      const b = lane.centre.sampleAt(s + h).t;
      out[i] = Math.abs(Math.atan2(a.x * b.y - a.y * b.x, a.x * b.x + a.y * b.y)) / (2 * h);
      peak = Math.max(peak, out[i]!);
    }
  }
  profiles.set(lane, out);
  peaks.set(lane, peak);
  return out;
}

// src/sim/vehicles/curvature.ts:52-55 slice of src/sim/vehicles/curvature.ts#straight (shared): the feature's statements; "…" marks code left out
function straight(lane: Lanelet): boolean {
  profileOf(lane);
  return peaks.get(lane)! < STRAIGHT;
}

// src/sim/vehicles/curvature.ts:73-81 slice of src/sim/vehicles/curvature.ts#slowestBend (shared): the feature's statements; "…" marks code left out
export function slowestBend(v: Vehicle, lane: Lanelet): number {
  // … 2 lines
  for (const k of profileOf(lane)) {
    if (k < 1e-6) continue;
    slowest = Math.min(slowest, bendSpeed(share, k));
  }
  return slowest;
}

// src/sim/vehicles/curvature.ts:84-126 slice of src/sim/vehicles/curvature.ts#curveSpeedCap (shared): the feature's statements; "…" marks code left out
export function curveSpeedCap(w: SimWorld, v: Vehicle): number {
  // … 19 lines
  while (lane && ahead < horizon) {
    // A straight lanelet - most of a town's - has no sample that could lower the cap
    if (!straight(lane)) {
      const profile = profileOf(lane);
      const start = Math.max(0, Math.floor(Math.max(0, from) / SAMPLE));
      for (let i = start; i < profile.length; i++) {
        const s = i * SAMPLE;
        const d = Math.max(0, ahead + s);
        if (d > horizon) break;
        const k = profile[i]!;
        if (k < STRAIGHT) continue;
        cap = Math.min(cap, before(bendSpeed(share, k), d));
      }
    }
    ahead += lane.length;
    from = 0;
    // The route lists the current lanelet first.
    let id = v.route[++next];
    while (id === lane.id) id = v.route[++next];
    lane = id ? w.lanelet(id) : undefined;
  }
  return cap;
}
