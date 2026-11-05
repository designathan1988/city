// The code of src/jobs/jobPool.ts that is the feature's (the rest of the file is shared).

// src/jobs/jobPool.ts:236-236 field JobSystem.firstLanes: the feature's own
  readonly firstLanes = new Set<JobService>();

// src/jobs/jobPool.ts:588-590 function laneFirst: the feature's own
export function laneFirst(service: JobService, on: boolean): void {
  if (on) system.firstLanes.add(service); else system.firstLanes.delete(service);
}
