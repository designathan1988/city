// The code of src/render/pagePorts.ts that is the feature's (the rest of the file is shared).

// src/render/pagePorts.ts:14-17 function laneOf: the feature's own
export function laneOf(service: JobService): Worker | null {
  const port = providers.get(service);
  return port ? port() : null;
}

// src/render/pagePorts.ts:20-20 function hasLanes: the feature's own
export const hasLanes = (service: JobService): boolean => providers.has(service);
