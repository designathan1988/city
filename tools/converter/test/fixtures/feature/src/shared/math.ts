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

/** A span of values, from lo to hi. */
export interface Range {
  readonly lo: number;
  readonly hi: number;
}

/** A figure of this file alone, read only through another value's text. */
const WIDE = 100;

/** The whole span a ferry sails. */
export const FULL: Range = { lo: 0, hi: WIDE };

/** Whether a value is inside a span. */
export function within(v: number, r: Range): boolean {
  return v >= r.lo && v <= r.hi;
}
