// The code of src/sim/vehicles/obstacles.ts that is the feature's (the rest of the file is shared).

// src/sim/vehicles/obstacles.ts:20-56 slice of src/sim/vehicles/obstacles.ts#pedestrianAhead (shared): the feature's statements; "…" marks code left out
function pedestrianAhead(w: SimWorld, v: Vehicle): Obstacle | null {
  // … 22 lines
  for (const crossing of crossingIdsAt(w, conn.node)) {
    const span = pedestrianInSpan(w, conn.id, crossing);
    if (!span) continue;
    // … 1 lines
    const buffered = offset - PED_CROSSING_STOP_BUFFER;
    const gap = lane.kind === 'link'
      ? Math.max(0, canStopComfortably(v.driver, v.v, buffered - v.driver.s0) ? buffered : offset)
      : offset + span.along - PED_STOP_MARGIN;
    // … 1 lines
    if (gap < 0) continue;
    if (!best || gap < best.gap) best = { gap: Math.max(0, gap), speed: 0, kind: 'pedestrian' };
  }
  return best;
}

// src/sim/vehicles/obstacles.ts:109-212 slice of src/sim/vehicles/obstacles.ts#longitudinalConstraints (shared): the feature's statements; "…" marks code left out
export function longitudinalConstraints(w: SimWorld, v: Vehicle): ConstraintSet {
  // … 75 lines
  if (lane.kind === 'link' && lane.controlled) {
    // … 1 lines
    if (!v.admittedConnector) {
      // … 4 lines
      if (conn && controller && junction?.signalised) {
        // … 1 lines
        const adjacentStop = w.crossingSpans.approachStop(lane.id);
        const signalGap = adjacentStop === undefined ? dStop :
          Math.min(dStop, Math.max(0, adjacentStop - v.s));
        // … 1 lines
        const mustStop = signalHolds(v, conn.id, state, conn.turn, signalGap);
        if (mustStop) {
          addObstacle(constraints, signalGap, 0, 'signal');
        }
      }
    }
  }
// … 7 lines
}
