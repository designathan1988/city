// The code of src/sim/vehicles/obstacles.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/obstacles.ts:109-212 slice of src/sim/vehicles/obstacles.ts#longitudinalConstraints (shared): the feature's statements; "…" marks code left out
export function longitudinalConstraints(w: SimWorld, v: Vehicle): ConstraintSet {
  // … 28 lines
  for (let k = 0; k < Math.min(3, v.route.length); k++) {
    const closed = w.city.transit.closedOn(v.route[k]!);
    if (!closed.length) continue;
    // … 7 lines
    for (const at of closed) {
      // Already over the line (its front past the track): it goes on across.
      if (offset + at <= 0) continue;
      addObstacle(constraints, Math.max(0, offset + at - CROSSING_STOP), 0, 'signal');
    }
  }
  // … 23 lines
  if (lane.kind === 'link' && lane.controlled && !v.admittedConnector) {
    // … 1 lines
    const track = w.city.transit.crossingNearEnd(lane.id, lane.length);
    if (track !== null && v.s < track) {
      addObstacle(constraints, Math.max(0, track - CROSSING_STOP - v.s), 0, 'signal');
    }
  }
// … 28 lines
}
