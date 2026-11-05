// The code of src/render/pagePorts.ts that is the feature's (the rest of the file is shared).

// src/render/pagePorts.ts:9-11 function provideLanes: the feature's own
export function provideLanes(service: JobService, port: () => Worker): void {
  providers.set(service, port);
}
