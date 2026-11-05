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

// src/editor/workerParts.ts:16-16 interface Answer
interface Answer { readonly id: number; readonly ok: boolean; readonly result?: unknown }

// src/editor/workerParts.ts:19-19 variable backend
/** This worker's store, once known; what to call when the port comes. */
let backend: Promise<PartStore> | null = null;

// src/editor/workerParts.ts:20-20 variable give
let give: ((store: PartStore) => void) | null = null;

// src/editor/workerParts.ts:25-32 function takeStoreSetup
/** A setup message taken in (`job.worker.ts`, the simulation's worker). */
export function takeStoreSetup(setup: StoreSetup): void;

// src/editor/workerParts.ts:35-42 function workerParts
/** The store of this worker: IndexedDB unless the page said it comes by a port. */
export function workerParts(): PartStore;

// src/editor/workerParts.ts:45-77 function portParts
/** The saves writer over a port (the requests of `desktop/saves.cjs`: 'get' and 'write', answered by id in the order asked). */
export function portParts(port: MessagePort): PartStore & { readonly drop: () => void };
