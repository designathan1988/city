// What the feature uses of src/bayPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/bayPool.ts:7-14 interface Lane
/** THE LOTS' BAYS WORKED OUT ON THE OTHER CORES (`bays.worker.ts`). */
interface Lane {
  readonly worker: Worker;
  readonly waiting: Map<number, { resolve(answer: Bay[][]): void; reject(error: Error): void }>;
  /** When the worker last spoke, or was handed a job (`performance.now()`). */
  heard: number;
  /** Whether this stretch of silence was already said. */
  told: boolean;
}

// src/bayPool.ts:17-17 variable SILENCE_MS
/** DO NOT take a silent worker for a dead one: it shares the job pool's workers and the CPU share; a failure comes as an error (`onerror`, an `error` answer). */
const SILENCE_MS = 10_000;

// src/bayPool.ts:22-124 class BayPool
export class BayPool implements BayWorkers { … }

// src/bayPool.ts:26-26 field BayPool.lanes
private lanes: Lane[] | null = null;

// src/bayPool.ts:32-50 method BayPool.work
work(asks: readonly LotAsk[]): Promise<Bay[][]>;

// src/bayPool.ts:61-95 method BayPool.start
private start(): Lane[] | null;
