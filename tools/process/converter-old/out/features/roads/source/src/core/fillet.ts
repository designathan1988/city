// The code of src/core/fillet.ts that is the feature's (the rest of the file is shared).

// src/core/fillet.ts:51-54 function acuteSetback: the feature's own
export function acuteSetback(psi: number, hwA: number, hwB: number): number {
  const s = Math.sin(Math.max(psi, COARSE_EPS) / 2);
  return (ACUTE_CLEARANCE * (hwA + hwB)) / (2 * s);
}

// src/core/fillet.ts:62-64 function curbRadius: the feature's own
export function curbRadius(hwA: number, hwB: number): number {
  return clamp(0.5 * Math.min(hwA, hwB), CURB_R_MIN, CURB_R_MAX);
}
