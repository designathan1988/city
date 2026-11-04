// Fixture: code the program and the feature both use.
export function clamp(v: number, lo: number, hi: number): number {
  return Math.min(hi, Math.max(lo, v));
}

/** Only the feature's code reaches this, but it sits in a shared file. */
export function roundTo(v: number, step: number): number {
  function snapTo(x: number): number {
    return Math.round(x / step) * step;
  }
  return snapTo(v);
}
