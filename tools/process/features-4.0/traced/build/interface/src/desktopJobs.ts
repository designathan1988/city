// What the feature uses of src/desktopJobs.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/desktopJobs.ts:15-46 function desktopJobWorkers
/** What makes the pool's workers on the desktop (`jobPool.ts` setJobWorkerMaker): a worker in a job process for the long side, none (the page's own) for the short; null where the page has no desktop shell, or one without job processes. */
export function desktopJobWorkers(): ((long: boolean) => WorkerLike | null) | null;
