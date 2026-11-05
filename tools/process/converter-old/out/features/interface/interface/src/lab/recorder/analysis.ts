// What the feature uses of src/lab/recorder/analysis.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/recorder/analysis.ts:19-29 interface Dist
/** A distribution of one measure over a stretch's frames. */
export interface Dist {
  readonly n: number;
  readonly mean: number;
  readonly std: number;
  readonly p50: number;
  readonly p95: number;
  readonly p99: number;
  readonly max: number;
  /** Median absolute deviation, scaled to a standard deviation (×1.4826): the spread, untouched by the hitches. */
  readonly spread: number;
}

// src/lab/recorder/analysis.ts:31-40 interface Settle
export interface Settle {
  /** ms after the change the settling lasted (`SETTLE_MS` at least). */
  readonly ms: number;
  /** It never settled before the next change or `SETTLE_MAX_MS`. */
  readonly never: boolean;
  readonly frames: number;
  readonly maxIntervalMs: number;
  readonly maxCpuMs: number;
  readonly maxGpuMs: number | null;
}

// src/lab/recorder/analysis.ts:42-62 interface Segment
export interface Segment {
  readonly index: number;
  readonly startMs: number;
  readonly endMs: number;
  /** What started it (none: the first, the baseline). */
  readonly causes: readonly RecEvent[];
  readonly label: string;
  readonly interval: Dist | null;
  readonly cpu: Dist | null;
  readonly gpu: Dist | null;
  readonly hitches25: number;
  readonly hitches50: number;
  readonly drawCalls: Dist | null;
  readonly triangles: Dist | null;
  /** MB a second the heap grew over the stretch (least squares), null without a heap reading. */
  readonly heapSlopeMBps: number | null;
  readonly heapMB: number | null;
  readonly settle: Settle | null;
  /** The frame's systems, mean ms, the costliest first. */
  readonly systems: readonly { readonly name: string; readonly meanMs: number }[];
}

// src/lab/recorder/analysis.ts:64-64 type Verdict
export type Verdict = 'improved' | 'worse' | 'within noise';

// src/lab/recorder/analysis.ts:66-74 interface Delta
export interface Delta {
  readonly metric: string;
  readonly from: number;
  readonly to: number;
  readonly delta: number;
  readonly pct: number | null;
  readonly noise: number;
  readonly verdict: Verdict;
}

// src/lab/recorder/analysis.ts:76-86 interface Comparison
export interface Comparison {
  readonly segment: number;
  readonly against: number;
  /** `previous` or `baseline`. */
  readonly kind: 'previous' | 'baseline';
  readonly deltas: readonly Delta[];
  /** The difference alone, said: the biggest real ones, or why there is none. */
  readonly effect: string;
  /** One line a reader takes in: the change, then `effect`. */
  readonly summary: string;
}
