// The code of src/sim/vehicleSignals.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicleSignals.ts:31-65 slice of src/sim/vehicleSignals.ts#indicatorSide (shared): the feature's statements; "…" marks code left out
export function indicatorSide(w: SimWorld, v: Vehicle): IndicatorSide {
  // … 6 lines
  if (wanted) {
    // … 1 lines
    if (target && here.kind === 'link') {
      // Into frames made once: every vehicle signalling a lane change, every frame drawn.
      const frame = here.centre.sampleAtInto(Math.min(Math.max(0, v.s), here.length), HERE);
      target.centre.closestInto(frame.p.x, frame.p.y, HIT);
      const at = target.centre.sampleAtInto(HIT.s, THERE).p;
      const leftward = (at.x - frame.p.x) * -frame.t.y + (at.y - frame.p.y) * frame.t.x;
      if (Math.abs(leftward) > 1e-6) return leftward > 0 ? 1 : -1;
    }
  }
  // … 1 lines
  if (here.kind === 'connector') {
    if (v.s / Math.max(here.length, 1e-6) < TURN_SIGNAL_UNTIL) return turnSide(here.turn);
    return 0;
  }
  // … 1 lines
  const toLine = here.length - v.s;
  if (toLine > Math.max(TURN_SIGNAL_MIN, v.v * TURN_SIGNAL_TIME)) return 0;
  for (const id of v.route) {
    // … 3 lines
    if (next.kind === 'connector') return turnSide(next.turn);
  // … 1 lines
  }
// … 1 lines
}

// src/sim/vehicleSignals.ts:95-112 slice of src/sim/vehicleSignals.ts#pathCurvature (shared): the feature's statements; "…" marks code left out
export function pathCurvature(w: SimWorld, v: Vehicle): number {
  // … 2 lines
  const span = Math.min(m(2), lane.length / 2);
  const s = Math.min(Math.max(v.s, span), lane.length - span);
  // … 1 lines
  if (span > 1e-3) {
    const a = lane.centre.sampleAtInto(s - span, BEFORE).t;
    const b = lane.centre.sampleAtInto(s + span, AFTER).t;
    const turn = Math.atan2(a.x * b.y - a.y * b.x, a.x * b.x + a.y * b.y);
    curvature = turn / (2 * span);
  }
  // … 1 lines
  if (travelled > 1e-4) {
    curvature += (Math.atan(v.lateralSlope) - Math.atan(v.prev.lateralSlope)) / travelled;
  }
  return curvature;
}
