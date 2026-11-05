// The code of src/sim/pose.ts that is the feature's (the rest of the file is shared).

// src/sim/pose.ts:165-187 slice of src/sim/pose.ts#bodyFrame (shared): the feature's statements; "…" marks code left out
function bodyFrame(w: SimWorld, vehicle: Vehicle, kinematics: Kinematics, behindFront: number, out: FrameOut): boolean {
  // … 4 lines
  if (s > lane.length) {
    const nextId = lane.kind === 'connector' ? lane.toLane
      : kinematics.lanelet !== vehicle.lanelet && w.connector(vehicle.lanelet)?.fromLane === lane.id
        ? vehicle.lanelet
        : vehicle.admittedConnector && w.connector(vehicle.admittedConnector)?.fromLane === lane.id
          ? vehicle.admittedConnector : undefined;
    const next = nextId ? w.lanelet(nextId) : undefined;
    if (next) { next.centre.sampleAtInto(s - lane.length, out); return true; }
  }
  if (s >= 0) { lane.centre.sampleAtInto(s, out); return true; }
  for (const id of kinematics.rearPath) {
    // … 2 lines
    s += lane.length;
    if (s >= 0) { lane.centre.sampleAtInto(s, out); return true; }
  }
// … 1 lines
}
