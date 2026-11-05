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

// src/jobs/jobPool.ts:69-69 interface Queued
/** A job in the queue: waiting, running on a worker, or over (answered, cancelled or replaced by a newer one of its key). */
interface Queued { readonly seq: number; readonly spec: JobSpec; state: 'waiting' | 'running' | 'over'; rank: number }

// src/jobs/jobPool.ts:208-227 interface Host
/** A worker of the pool: the lanes it hosts (`place`), its port per service for queued jobs, the queued job it runs, whether it failed. */
interface Host {
  readonly worker: Worker;
  /** A worker of the long-running services (`longWorkers`) or of every other kind: never both. */
  readonly long: boolean;
  /** A thread of the page's own process, in the page's V8 cage (`pageRoomWorkers`); not a desktop job process's. */
  readonly inPage: boolean;
  /** Since when it has had nothing on it (`idle`), NaN while it has. */
  freeSince: number;
  ports: number;
  readonly services: Map<JobService, number>;
  readonly queuePorts: Map<JobService, number>;
  job: Queued | null;
  failed: boolean;
  /** Messages its lanes posted and not yet answered (`JobPort`): a queued job sent there waits behind them. */
  laneWork: number;
  /** Of `laneWork`, those of a lane that goes first (`laneFirst`): no queued job is given the worker while it has any. */
  firstWork: number;
  /** Told when it fails: the lanes it serves for another thread (`jobChannel`), whose ports say nothing of it. */
  readonly channelLost: (() => void)[];
}

// src/jobs/jobPool.ts:230-248 class JobSystem
/** The pool of this thread: its workers (made as work waits, `grow`), its lanes by port, its queue and the point the view looks at. */
class JobSystem { … }

// src/jobs/jobPool.ts:232-232 field JobSystem.ports
readonly ports = new Map<number, JobPort>();

// src/jobs/jobPool.ts:233-233 field JobSystem.nextPort
nextPort = 1;

// src/jobs/jobPool.ts:249-249 variable system
const system = new JobSystem();

// src/jobs/jobPool.ts:488-488 method submitJob.cancel
cancel: () => …;

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

// src/jobs/jobPool.ts:534-534 function canQueueJobs
/** Whether this thread can run queued jobs at all (Node's tests have no `Worker`): the asker does them itself otherwise. */
export const canQueueJobs = (): boolean => …;

// src/jobs/jobPool.ts:559-581 function place
/** DO NOT pin a lane by a hash of its service, nor put a long-running service's lane on a worker of the other kinds (or the other way round): the traffic's conflict pairs waited behind tile after tile and the city's clock stood still (the long lanes were placed last, on the workers the traffic's lanes already held) */
function place(service: JobService): Host;

// src/jobs/jobPool.ts:593-660 class JobPort
/** A kind of work's lane in the pool: answers as a `Worker` does (`onmessage`, `onerror`, `postMessage`, `terminate`). */
export class JobPort { … }

// src/jobs/jobPool.ts:597-597 field JobPort.worker
readonly worker: Worker;

// src/jobs/jobPool.ts:599-599 field JobPort.host
private readonly host: Host;

// src/jobs/jobPool.ts:669-671 function jobPort
/** A port of the pool for one of `service`'s lanes (`jobLanes` lanes at most), typed as the `Worker` its pool was written for. */
export function jobPort(service: JobService): Worker;
