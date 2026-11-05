// What the feature uses of src/editor/workerParts.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/workerParts.ts:6-11 interface StoreSetup
/** THE SAVE STORE AS A WORKER OPENS IT: the browser's IndexedDB, or on the desktop the shell's saves writer (`desktop/saves.cjs`) through a port of the worker's own that the page handed it (`desktopSaves.ts` `connect`) - never the page in between, so the desktop's workers read and write the tiles as a browser's do */

/** What the page posts a worker of its own about its store, before any job (`jobs/jobPool.ts` `setJobWorkerSetup`): that it comes by a port, then the port. */
export interface StoreSetup {
  readonly storeSetup: true;
  /** The store comes by a port: wait for it, DO NOT open IndexedDB (on the desktop it is not the save). */
  readonly byPort?: boolean;
  readonly port?: MessagePort;
}

// src/editor/workerParts.ts:13-14 function isStoreSetup
export const isStoreSetup = (data: unknown): data is StoreSetup => …;

// src/editor/workerParts.ts:25-32 function takeStoreSetup
/** A setup message taken in (`job.worker.ts`, the simulation's worker). */
export function takeStoreSetup(setup: StoreSetup): void;
