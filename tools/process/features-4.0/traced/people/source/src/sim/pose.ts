// The code of src/sim/pose.ts that is the feature's (the rest of the file is shared).

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

// src/sim/pose.ts:199-206 slice of src/sim/pose.ts#pedPoseInto (shared): the feature's statements; "…" marks code left out
export function pedPoseInto(p: PedView, alpha: number, out: PoseOut): void {
  // … 1 lines
  const bx = p.prev.x, by = p.prev.y;
  if (Math.hypot(p.x - bx, p.y - by) > POSE_JUMP_LIMIT) { out.p.x = p.x; out.p.y = p.y; out.angle = p.heading; return; }
  out.p.x = bx + (p.x - bx) * t;
  out.p.y = by + (p.y - by) * t;
  out.angle = lerpAngle(p.prev.heading, p.heading, t);
}
