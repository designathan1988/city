// What the feature uses of src/jobs/jobPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/jobs/jobPool.ts:494-508 interface JobWorkerStats
/** One worker of the pool as the debugging lab reads it (`lab/resources`). */
export interface JobWorkerStats {
  readonly long: boolean;
  readonly inPage: boolean;
  /** The service of the queued job it runs now; null: none. */
  readonly running: string | null;
  /** That job's key (`JobSpec.key`): which cell, tile or city it is. */
  readonly runningKey: string | null;
  /** The services it hosts lanes of, and how many lanes each. */
  readonly lanes: readonly (readonly [string, number])[];
  /** Messages its lanes posted and not yet answered. */
  readonly laneWork: number;
  readonly failed: boolean;
  /** How long it has stood with nothing on it, ms; null while it has work (or was not looked at since). */
  readonly idleMs: number | null;
}

// src/jobs/jobPool.ts:511-531 function jobPoolStats
/** The pool as it stands, read only: its workers, the queue's waiting jobs, the most it may hold (the debugging lab's consumers; nothing is changed). */
export function jobPoolStats(): { readonly most: number; readonly longMost: number; readonly shortMost: number; readonly waiting: number; readonly submitted: number; readonly ports: number; readonly workers: readonly JobWorkerStats[] };
