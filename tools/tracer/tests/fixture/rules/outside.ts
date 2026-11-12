// Names no scope gives are the platform's objects: none of their members is the code's, even where a class of the
// code has a member of that name.
export class Tally {
  max(): number { return 1; }
  isFinite(): boolean { return true; }
}

export function measure(t: Tally): number {
  const m = Ledger.max(1, 2);
  return Sheet.isFinite(m) ? t.max() : 0;
}
