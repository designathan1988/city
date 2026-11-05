// What the feature uses of src/jobs/jobPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/jobs/jobPool.ts:40-55 interface JobSpec
/** A job of the central queue (Cidade's job pool, `submit`): its message written when a worker takes it, answered once. */
interface JobSpec {
  readonly service: JobService;
  /** A newer job with the same key replaces this one: never sent if still waiting, its answer dropped if begun. */
  readonly key: string | null;
  /** Lower first: the job's distance to the eye (`focusDistance`), in metres, or below it for what must come first; with `at`, its band (what comes before the distance, the same for a kind of job). */
  readonly priority: number;
  /** Where on the map the job's work stands: its rank is `priority` plus its distance to where the view looks NOW, re-ranked as the view moves (`setJobFocus`), not where it looked when the job was asked. */
  readonly at?: readonly [number, number];
  /** The message, written only when a worker takes the job (a job waiting its turn holds no copy of it); `to`, the worker taking it. */
  readonly write: (to: JobTaker) => { readonly data: unknown; readonly transfer: Transferable[] };
  /** Fit for a worker in a desktop job process (`processWorker.ts`): its message holds only what the job reads (`cellPool.ts` the ground's plates around it), copied there. Without it the job waits for the page's own workers. */
  readonly crosses?: boolean;
  readonly done: (answer: unknown) => void;
  /** Its worker failed, or none can run it: the asker does it another way. */
  readonly failed: (message: string) => void;
}

// src/jobs/jobPool.ts:58-58 interface JobTaker
/** The worker a job's message is written for: whether it shares the page's memory (a worker of the page), or every shared buffer of the message is copied to it (a desktop job process's, `processWorker.ts` unshared). */
export interface JobTaker { readonly sharesMemory: boolean }

// src/jobs/jobPool.ts:61-66 interface JobTicket
/** A job of the queue as its asker holds it. */
export interface JobTicket {
  /** Not wanted any more: never sent if waiting, its answer dropped if begun. */
  cancel(): void;
  /** Waiting or running, not answered, cancelled nor replaced. */
  readonly pending: boolean;
}

// src/jobs/jobPool.ts:257-260 function setJobWorkerSetup
/** Told to every worker of the pool, those made and those to come, before its first job: on the desktop, the port it reads and writes the tiles through (`editor/workerParts.ts`). */
export function setJobWorkerSetup(setup: (worker: Worker) => void): void;

// src/jobs/jobPool.ts:484-491 function submitJob
/** A job for the queue: run by the first free worker, nearest the eye first (`JobSpec`). */
export function submitJob(spec: JobSpec): JobTicket;

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

// src/jobs/jobPool.ts:669-671 function jobPort
/** A port of the pool for one of `service`'s lanes (`jobLanes` lanes at most), typed as the `Worker` its pool was written for. */
export function jobPort(service: JobService): Worker;
