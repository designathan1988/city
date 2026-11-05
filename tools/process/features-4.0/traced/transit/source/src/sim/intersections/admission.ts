// The code of src/sim/intersections/admission.ts that is the feature's (the rest of the file is shared).

// src/sim/intersections/admission.ts:98-214 slice of src/sim/intersections/admission.ts#admit (shared): the feature's statements; "…" marks code left out
function admit(w: SimWorld, banker: BankerPass): void {
  // … 3 lines
  for (const node of w.doLess ? w.junctionsApproached() : w.junctionNodesInOrder()) {
    // … 3 lines
    for (const laneId of junction.inbound) {
      // … 12 lines
      const track = w.city.transit.crossingNearEnd(laneId, lane.length);
      const before = track === null ? 0 : lane.length - track + CROSSING_STOP + head.driver.s0 + m(2);
      if (d > Math.max(REQUEST_MIN_DISTANCE, head.v * REQUEST_TIME, before)) continue;
    // … 17 lines
    }
  }
// … 73 lines
}
