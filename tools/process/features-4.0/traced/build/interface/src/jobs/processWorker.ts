// What the feature uses of src/jobs/processWorker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/jobs/processWorker.ts:4-10 interface WorkerLike
/** A JOB WORKER IN ANOTHER PROCESS (the desktop's, `desktop/jobs.cjs`): the shape of a `Worker` the pool was written for (`jobPool.ts` grow), over a port to a thread of a utility process, so its heap is in that process's V8 cage, not in the page's (Electron `utilityProcess`; V8 pointer compression: one cage a process) */

/** What the pool uses of a worker. */
export interface WorkerLike {
  onmessage: ((event: MessageEvent) => void) | null;
  onerror: ((event: ErrorEvent) => void) | null;
  onmessageerror: ((event: MessageEvent) => void) | null;
  postMessage(message: unknown, transfer?: Transferable[]): void;
  terminate(): void;
}

// src/jobs/processWorker.ts:112-191 class ProcessWorker
/** A job worker whose thread is in another process: messages posted before its port came are held, then sent in order; its port given by `connect` (one port a worker, `desktopJobs.ts`). */
export class ProcessWorker implements WorkerLike { … }
