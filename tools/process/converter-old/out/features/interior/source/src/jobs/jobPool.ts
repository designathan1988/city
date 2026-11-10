// The code of src/jobs/jobPool.ts that is the feature's (the rest of the file is shared).

// src/jobs/jobPool.ts:537-539 function warmJobWorkers: the feature's own
export function warmJobWorkers(): void {
  if (system.workers.length === 0) grow(false);
}
