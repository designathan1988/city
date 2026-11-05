// The code of src/sim/pose.ts that is the feature's (the rest of the file is shared).

// src/sim/pose.ts:39-43 slice of src/sim/pose.ts#bodyOffset (shared): the feature's statements; "…" marks code left out
function bodyOffset(k: Kinematics, length: number): number {
  if (k.lateralSlope === 0) return k.lateral;
  const sine = k.lateralSlope / Math.sqrt(1 + k.lateralSlope * k.lateralSlope);
  return k.lateral + REAR_AXLE_TO_CENTRE * length * sine;
}

// src/sim/pose.ts:46-49 slice of src/sim/pose.ts#vehiclePose (shared): the feature's statements; "…" marks code left out
export function vehiclePose(w: SimWorld, v: Vehicle, alpha: number): Pose | null {
  // … 1 lines
  return vehiclePoseInto(w, v, alpha, out) ? out : null;
}

// src/sim/pose.ts:52-84 slice of src/sim/pose.ts#vehiclePoseInto (shared): the feature's statements; "…" marks code left out
export function vehiclePoseInto(w: SimWorld, v: Vehicle, alpha: number, out: PoseOut): boolean {
  // … 1 lines
  const free = v.free;
  // … 1 lines
  if (free) {
    out.p.x = lerp(free.px, free.x, t); out.p.y = lerp(free.py, free.y, t);
    out.angle = lerpAngle(free.pangle, free.angle, t);
    return true;
  }
  const frame = axleFrame(w, v, v, v.archetype.length, AXLE_NOW);
  if (!frame) return false;
  // … 2 lines
  const side = bodyOffset(v, v.archetype.length) + cycleShift(v, w.lanelet(v.lanelet), v.s);
  // … 1 lines
  const atX = frame.p.x - frame.t.y * side, atY = frame.p.y + frame.t.x * side;
  const heading = Math.atan2(frame.t.y, frame.t.x);
  out.p.x = atX; out.p.y = atY; out.angle = heading;
  // … 1 lines
  const before = axleFrame(w, v, v.prev, v.archetype.length, AXLE_BEFORE);
  if (!before) return true;
  const sideBefore = bodyOffset(v.prev, v.archetype.length) + cycleShift(v, w.lanelet(v.prev.lanelet), v.prev.s);
  BEFORE_OWN.x = before.p.x - before.t.y * sideBefore; BEFORE_OWN.y = before.p.y + before.t.x * sideBefore;
  const beforeAt = BEFORE_OWN;
  const beforeAhead = before.t;
  if (Math.hypot(beforeAt.x - atX, beforeAt.y - atY) > POSE_JUMP_LIMIT) return true;
  // … 2 lines
  const yaw = lerp(laneChangeYaw(v.prev.lateralSlope), laneChangeYaw(v.lateralSlope), t);
  out.p.x = lerp(beforeAt.x, atX, t); out.p.y = lerp(beforeAt.y, atY, t);
  out.angle = lerpAngle(Math.atan2(beforeAhead.y, beforeAhead.x), heading, t) + yaw;
// … 1 lines
}

// src/sim/pose.ts:96-119 slice of src/sim/pose.ts#vehicleFramesInto (shared): the feature's statements; "…" marks code left out
export function vehicleFramesInto(w: SimWorld, v: Vehicle, out: VehicleFrames): boolean {
  const free = v.free;
  if (free) {
    out.bx = free.px; out.by = free.py; out.bangle = free.pangle;
    out.x = free.x; out.y = free.y; out.angle = free.angle;
    out.yawBefore = 0; out.yawNow = 0; out.jump = false;
    return true;
  }
  const frame = axleFrame(w, v, v, v.archetype.length, AXLE_NOW);
  if (!frame) return false;
  const side = bodyOffset(v, v.archetype.length) + cycleShift(v, w.lanelet(v.lanelet), v.s);
  out.x = frame.p.x - frame.t.y * side; out.y = frame.p.y + frame.t.x * side;
  out.angle = Math.atan2(frame.t.y, frame.t.x);
  // … 1 lines
  const before = axleFrame(w, v, v.prev, v.archetype.length, AXLE_BEFORE);
  if (!before) { out.jump = true; out.bx = out.x; out.by = out.y; out.bangle = out.angle; return true; }
  const sideBefore = bodyOffset(v.prev, v.archetype.length) + cycleShift(v, w.lanelet(v.prev.lanelet), v.prev.s);
  out.bx = before.p.x - before.t.y * sideBefore; out.by = before.p.y + before.t.x * sideBefore;
  out.bangle = Math.atan2(before.t.y, before.t.x);
  // … 1 lines
  out.yawBefore = laneChangeYaw(v.prev.lateralSlope);
  out.yawNow = laneChangeYaw(v.lateralSlope);
// … 1 lines
}

// src/sim/pose.ts:133-142 function vehiclePoseNowInto: the feature's own
export function vehiclePoseNowInto(w: SimWorld, v: Vehicle, out: PoseOut): boolean {
  const free = v.free;
  if (free) { out.p.x = free.x; out.p.y = free.y; out.angle = free.angle; return true; }
  const frame = axleFrame(w, v, v, v.archetype.length, AXLE_NOW);
  if (!frame) return false;
  const side = bodyOffset(v, v.archetype.length) + cycleShift(v, w.lanelet(v.lanelet), v.s);
  out.p.x = frame.p.x - frame.t.y * side; out.p.y = frame.p.y + frame.t.x * side;
  out.angle = Math.atan2(frame.t.y, frame.t.x) + laneChangeYaw(v.lateralSlope);
  return true;
}

// src/sim/pose.ts:151-162 slice of src/sim/pose.ts#axleFrame (shared): the feature's statements; "…" marks code left out
function axleFrame(w: SimWorld, vehicle: Vehicle, kinematics: Kinematics, length: number, out: FrameOut): FrameOut | null {
  if (!bodyFrame(w, vehicle, kinematics, length / 2, out)) return null;
  if (!bodyFrame(w, vehicle, kinematics, length / 2 - HEADING_CHORD, AHEAD) || !bodyFrame(w, vehicle, kinematics, length / 2 + HEADING_CHORD, BEHIND)) return out;
// … 8 lines
}

// src/sim/pose.ts:165-187 slice of src/sim/pose.ts#bodyFrame (shared): the feature's statements; "…" marks code left out
function bodyFrame(w: SimWorld, vehicle: Vehicle, kinematics: Kinematics, behindFront: number, out: FrameOut): boolean {
  let lane = w.lanelet(kinematics.lanelet);
  if (!lane) return false;
  let s = kinematics.s - behindFront;
  // … 1 lines
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
    lane = w.lanelet(id);
    if (!lane) return false;
    s += lane.length;
    if (s >= 0) { lane.centre.sampleAtInto(s, out); return true; }
  }
// … 1 lines
}
