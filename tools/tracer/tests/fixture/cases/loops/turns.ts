class Tally { count = 0; odd = false; low = false; }

export function spin(t: Tally, n: number): void {
  let i = 0;
  const first = t.count;
  while (i < n) {
    if (i < 0) t.odd = true;
    t.count = t.count + 2;
    i = i + 1;
  }
  if (t.count < first || i < 0) t.low = true;
}
