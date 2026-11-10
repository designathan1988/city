// The code of src/sim/params.ts that is the feature's (the rest of the file is shared).

// src/sim/params.ts:25-30 function laneChangeLength: the feature's own
export function laneChangeLength(start: number, speed: number, bodyLength: number): number {
  const t = Math.min(1, Math.max(0, speed / LANE_CHANGE_FAST_SPEED));
  const heading = LANE_CHANGE_HEADING_SLOW + (LANE_CHANGE_HEADING_FAST - LANE_CHANGE_HEADING_SLOW) * t;
  return Math.max(PROFILE_PEAK * Math.abs(start) / Math.tan(heading),
    Math.max(0, speed) * LANE_CHANGE_MIN_TIME, bodyLength * LANE_CHANGE_MIN_BODIES, 1e-6);
}

// src/sim/params.ts:33-37 function laneChangeOffset: the feature's own
export function laneChangeOffset(start: number, travelled: number, length: number): number {
  const u = Math.min(1, Math.max(0, travelled / length));
  const done = u * u * u * (10 - 15 * u + 6 * u * u);
  return u >= 1 ? 0 : start * (1 - done);
}

// src/sim/params.ts:40-44 function laneChangeSlope: the feature's own
export function laneChangeSlope(start: number, travelled: number, length: number): number {
  const u = Math.min(1, Math.max(0, travelled / length));
  if (u >= 1) return 0;
  return (-start * 30 * u * u * (1 - u) * (1 - u)) / length;
}
