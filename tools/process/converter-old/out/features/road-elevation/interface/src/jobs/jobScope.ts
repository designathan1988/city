// What the feature uses of src/jobs/jobScope.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/jobs/jobScope.ts:2-5 interface JobScope
/** What a service of the job system is given in place of a worker's own global (`jobWorker.ts`): its messages, and a way to answer. */
export interface JobScope {
  onmessage: ((event: MessageEvent) => void) | null;
  postMessage(message: unknown, transfer?: Transferable[]): void;
}
