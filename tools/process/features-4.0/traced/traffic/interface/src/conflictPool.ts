// What the feature uses of src/conflictPool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/conflictPool.ts:8-13 interface Lane
/** THE TRAFFIC'S CONFLICT PAIRS, MEASURED ON THE OTHER CORES. */
interface Lane {
  readonly worker: Worker;
  readonly waiting: Map<number, { resolve(answer: unknown[]): void; reject(error: Error): void }>;
  /** Reports the worker silent with jobs waiting (`SILENCE_MS`). */
  watchdog: ReturnType<typeof setTimeout> | null;
}

// src/conflictPool.ts:24-24 constructor ConflictPool.constructor
/** `keepPrefix`: the key prefix the tables are kept under for the next session */
  /** `ports`: the page's lanes for this thread (`jobPool.ts` jobChannel, the simulation's worker): DO NOT open a pool here when given them. None: lanes of this thread's own pool. */
  constructor(private readonly keepPrefix: string | null = null, private readonly ports: readonly MessagePort[] | null = null);

// src/conflictPool.ts:61-69 method ConflictPool.ask
private ask(lane: Lane, sweeps: readonly object[], pairs: Int32Array, keep: { prefix: string; keys: readonly string[] } | null): Promise<(Float64Array | null)[]>;

// src/conflictPool.ts:103-133 method ConflictPool.sampleSweeps
/** Movements' sweeps sampled across the workers (`conflictTable.worker.ts`, `sampleSweep`) */
  sampleSweeps(asks: readonly SweepAsk[]): Promise<readonly SampledSweep[]>;

// src/conflictPool.ts:136-162 method ConflictPool.finishPairs
/** Pairs finished across the workers (`conflictTable.worker.ts`, `finishPair`) */
  finishPairs(pairs: readonly FinishAsk[]): Promise<readonly FinishedPair[]>;

// src/conflictPool.ts:165-181 method ConflictPool.buildJunctions
/** A whole build's junctions built across the workers (`conflictTable.worker.ts`, `buildJunctionOf`) */
  buildJunctions(asks: readonly JunctionAsk[]): Promise<readonly JunctionAnswer[]>;

// src/conflictPool.ts:210-249 method ConflictPool.start
private start(): Lane[] | null;
