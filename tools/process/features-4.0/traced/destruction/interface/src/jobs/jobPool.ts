// What the feature uses of src/jobs/jobPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/jobs/jobPool.ts:669-671 function jobPort
/** A port of the pool for one of `service`'s lanes (`jobLanes` lanes at most), typed as the `Worker` its pool was written for. */
export function jobPort(service: JobService): Worker;
