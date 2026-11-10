// The code of src/sim/ambient/demand.ts that is the feature's (the rest of the file is shared).

// src/sim/ambient/demand.ts:55-62 slice of src/sim/ambient/demand.ts#kindOf (shared): the feature's statements; "…" marks code left out
export function kindOf(b: Pick<Building, 'use' | 'function'>): keyof Mix {
  const fn = b.function ?? '';
  if (/house|apartment|residential|townhouse|flat/i.test(fn)) return 'home';
  if (/shop|market|mall|restaurant|cafe|bar|bakery|pharmacy|bank|hotel|cinema/i.test(fn)) return 'shop';
  if (/factory|warehouse|industrial|works|office/i.test(fn)) return 'work';
  if (fn) return 'civic';
  return b.use === 'commercial' ? 'shop' : b.use === 'industrial' ? 'work' : b.use === 'residential' ? 'home' : 'civic';
}
