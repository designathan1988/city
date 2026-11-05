// The code of src/render/platePaint.ts that is the feature's (the rest of the file is shared).

// src/render/platePaint.ts:9-9 slice of src/render/platePaint.ts#dabKey (shared): the feature's statements; "…" marks code left out
const dabKey = (d: PaintDab): string => `${d.kind}:${d.x}:${d.y}:${d.radius}:${d.strength}`;

// src/render/platePaint.ts:10-11 slice of src/render/platePaint.ts#dabInRects (shared): the feature's statements; "…" marks code left out
const dabInRects = (d: PaintDab, rects: readonly ChangeRect[]): boolean =>
  rects.some((r) => d.x + d.radius >= r[0] && d.x - d.radius <= r[2] && d.y + d.radius >= r[1] && d.y - d.radius <= r[3]);

// src/render/platePaint.ts:14-35 slice of src/render/platePaint.ts#plateDabs (shared): the feature's statements; "…" marks code left out
export function plateDabs(laid: readonly PaintDab[], keys: Set<string>, reaching: readonly PaintDab[], edited: readonly ChangeRect[] | null): PaintDab[] | null {
  if (edited === null) {
    if (sameList(reaching, laid)) return null;
    // … 1 lines
    for (const d of reaching) keys.add(dabKey(d));
    return [...reaching];
  }
  const here = new Set(reaching);
  const kept = edited.length === 0 ? laid : laid.filter((d) => here.has(d) || !dabInRects(d, edited));
  if (kept.length !== laid.length) { keys.clear(); for (const d of kept) keys.add(dabKey(d)); }
  const known = new Set(kept);
  const fresh: PaintDab[] = [];
  for (const d of reaching) {
    if (known.has(d)) continue;
    const key = dabKey(d);
    if (keys.has(key)) continue;
    keys.add(key);
    fresh.push(d);
  }
  if (kept.length === laid.length && fresh.length === 0) return null;
  return [...kept, ...fresh];
}
