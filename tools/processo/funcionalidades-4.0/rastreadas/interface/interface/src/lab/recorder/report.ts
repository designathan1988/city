// What the feature uses of src/lab/recorder/report.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/lab/recorder/report.ts:10-14 interface Anomaly
export interface Anomaly {
  readonly kind: 'hitch' | 'settle' | 'heap' | 'recompile' | 'longtask';
  readonly t: number;
  readonly detail: string;
}

// src/lab/recorder/report.ts:16-26 interface LabReport
export interface LabReport {
  /** File name stem: `rec-2026-10-19T12-30-05`. */
  readonly stamp: string;
  readonly json: Record<string, unknown>;
  readonly md: string;
  readonly segments: readonly Segment[];
  readonly comparisons: readonly Comparison[];
  readonly anomalies: readonly Anomaly[];
  /** One line for lists. */
  readonly summary: string;
}

// src/lab/recorder/report.ts:102-262 function buildReport
export function buildReport(ctx: LabContext, rec: Recording): LabReport;
