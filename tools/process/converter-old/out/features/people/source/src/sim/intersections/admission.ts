// The code of src/sim/intersections/admission.ts that is the feature's (the rest of the file is shared).

// src/sim/intersections/admission.ts:98-214 slice of src/sim/intersections/admission.ts#admit (shared): the feature's statements; "…" marks code left out
function admit(w: SimWorld, banker: BankerPass): void {
  // … 58 lines
  for (const r of requests) {
    // … 1 lines
    if (verdict.ok) {
    // … 46 lines
    } else {
      // … 2 lines
      const held = r.d - PED_CROSSING_STOP_BUFFER;
      const pedestrianGap = canStopComfortably(r.v.driver, r.v.v, held - r.v.driver.s0) ? held : r.d;
      addObstacle(r.v.constraints, Math.max(0, verdict.reason === 'pedestrian' ? pedestrianGap : r.d), 0, verdict.reason ?? 'signal');
    }
  }
}

// src/sim/intersections/admission.ts:979-983 slice of src/sim/intersections/admission.ts#stoppedForWalker (shared): the feature's statements; "…" marks code left out
function stoppedForWalker(v: Vehicle, toLine: number): boolean {
  // … 1 lines
  for (const o of v.constraints.obstacles) if (o.kind === 'pedestrian' && o.gap <= toLine + PED_BODY) return true;
// … 1 lines
}

// src/sim/intersections/admission.ts:986-991 slice of src/sim/intersections/admission.ts#crossingBusy (shared): the feature's statements; "…" marks code left out
function crossingBusy(w: SimWorld, conn: Connector): boolean {
  for (const leg of crossingLegs(w, conn.node)) {
    if (pedestrianInSpan(w, conn.id, leg.id)) return true;
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:1017-1044 slice of src/sim/intersections/admission.ts#crossingReachedFirst (shared): the feature's statements; "…" marks code left out
function crossingReachedFirst(w: SimWorld, r: Request): boolean {
  // … 5 lines
  for (const leg of crossingLegs(w, r.conn.node)) {
    // … 1 lines
    const occupants = w.crossingStates.get(crossing)?.occupants;
    if (!occupants?.length) continue;
    const span = w.crossingSpans.span(r.conn.id, crossing);
    if (!span) continue;
    // … 2 lines
    let front = CLEAR_MARGIN * travelTime(Math.max(0, r.d) + span.along, r.v.v, top, r.v.driver.a);
    // … 1 lines
    front = Math.max(front, aheadOnMovement(w, r, span.along, top));
    for (const p of occupants) {
      // Somebody held still on the zebra is waiting for something - very often for this very car - and is not arriving.
      if (p.held) continue;
      const at = p.s;
      const ahead = p.forward ? span.s0 - PED_BODY - at : at - span.s1 - PED_BODY;
      if (ahead <= 0) continue;
      if (ahead / Math.max(p.v, PED_MIN_PACE) < front + PED_REACH_TIME) return true;
    }
  }
// … 1 lines
}

// src/sim/intersections/admission.ts:1069-1100 slice of src/sim/intersections/admission.ts#pedestrianHasPriority (shared): the feature's statements; "…" marks code left out
function pedestrianHasPriority(w: SimWorld, r: Request): boolean {
  // … 3 lines
  for (const { segment, id } of crossingLegs(w, r.conn.node)) {
    // … 1 lines
    if (!state || (state.waitingFrom === 0 && state.waitingTo === 0)) continue;
    // … 1 lines
    const edge = w.sidewalks.edges.get(w.sidewalks.crossings.get(id) ?? '');
    if (edge && vehicleBodyIntersectsCrossing(w, r.v, edge)) continue;
    const span = w.crossingSpans.span(r.conn.id, id);
    if (span === null) continue;
    // … 1 lines
    if (!signalised || !controller) {
      // … 3 lines
      if (r.v.waited > PED_COURTESY) continue;
      // … 1 lines
      if (span) {
        const reach = PED_MIN_PACE * PED_REACH_TIME;
        const near = (state.waitingFrom > 0 && span.s0 < reach) || (state.waitingTo > 0 && state.length - span.s1 < reach);
        if (!near) continue;
      }
    // … 1 lines
    }
    // … 1 lines
    if (r.v.waited > PED_COURTESY) continue;
    if (pedestrianSignalState(controller, id, state.length) === 'walk') return true;
  }
// … 1 lines
}
