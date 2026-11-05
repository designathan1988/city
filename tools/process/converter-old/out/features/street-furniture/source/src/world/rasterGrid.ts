// The code of src/world/rasterGrid.ts that is the feature's (the rest of the file is shared).

// src/world/rasterGrid.ts:25-31 function firstTrue: the feature's own
export function firstTrue(lo: number, hi: number, pred: (i: number) => boolean): number {
  while (lo < hi) {
    const mid = (lo + hi) >>> 1;
    if (pred(mid)) hi = mid; else lo = mid + 1;
  }
  return lo;
}
