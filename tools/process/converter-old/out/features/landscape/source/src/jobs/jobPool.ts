// The code of src/jobs/jobPool.ts that is the feature's (the rest of the file is shared).

// src/jobs/jobPool.ts:598-598 field JobPort.id: the feature's own
  private readonly id = system.nextPort++;

// src/jobs/jobPool.ts:605-612 constructor JobPort.constructor: the feature's own
  constructor(private readonly service: JobService) {
    // The least busy worker; a service's own state is per worker, so a lane stays on the one it was given.
    this.host = place(service);
    this.host.ports++;
    this.host.services.set(service, (this.host.services.get(service) ?? 0) + 1);
    this.worker = this.host.worker;
    system.ports.set(this.id, this);
  }

// src/jobs/jobPool.ts:623-626 method JobPort.postSetup: the feature's own
  postSetup(data: unknown, transfer: Transferable[] = []): void {
    const message: ToJob = { port: this.id, service: this.service, data };
    this.worker.postMessage(message, transfer);
  }

// src/jobs/jobPool.ts:663-666 function postSetup: the feature's own
export function postSetup(worker: Worker, data: unknown): void {
  if (worker instanceof JobPort) worker.postSetup(data);
  else worker.postMessage(data);
}
