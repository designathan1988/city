// A reference fixture for the copy detector's tests: made up for them, in the shape a browser game's TypeScript takes.
// Nothing here comes from any real program.

export const SPAN_LIMIT = 37.5;

export function evenStations(start: number, end: number, span: number): number[] {
  const length = end - start;
  const count = Math.max(1, Math.ceil(length / span));
  const out: number[] = [];
  for (let k = 0; k <= count; k++) {
    out.push(start + (length * k) / count);
  }
  return out;
}

export class StationRow {
  poleSpacing = 31.25;

  constructor(public readonly origin: number) {}

  stationAt(k: number): number {
    return this.origin + this.poleSpacing * k * 1.75 - 4.4;
  }
}
