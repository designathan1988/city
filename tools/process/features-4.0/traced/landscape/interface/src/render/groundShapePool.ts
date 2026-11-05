// What the feature uses of src/render/groundShapePool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/groundShapePool.ts:6-13 interface Lane
/** DO NOT take a silent worker for a dead one: it shares the job pool's workers and the CPU share (a failure comes as an error). */
interface Lane {
  readonly worker: Worker;
  readonly waiting: Map<number, { resolve(answer: GroundJobAnswer): void; reject(error: Error): void }>;
  /** When the worker last answered, or was handed a job (`performance.now()`). */
  heard: number;
  /** Whether this stretch of silence was already said. */
  told: boolean;
}

// src/render/groundShapePool.ts:21-30 interface GroundWorkers
/** What does the ground's work off the page (`renderer.ts` `shapeGround`, `changedBlocks`). */
interface GroundWorkers {
  /** Whether jobs can be handed off the page at all (false: no workers here, or the pool failed). */
  readonly available: boolean;
  shape(ask: GroundShapeAsk): Promise<PlateShapeAnswer[]>;
  digest(ask: BlockDigestAsk): Promise<Float64Array[]>;
  /** Some buildings' platforms (`groundShape.ts` `padsOfAsk`). */
  pads(ask: PadsAsk): Promise<(Pad | null)[]>;
  /** Called while waiting: reports a worker with jobs waiting silent too long (queued behind other work or without CPU, still waited for). */
  check(now?: number): void;
}

// src/render/groundShapePool.ts:41-43 method GroundShapePool.shape
shape(ask: GroundShapeAsk): Promise<PlateShapeAnswer[]>;

// src/render/groundShapePool.ts:45-47 method GroundShapePool.digest
digest(ask: BlockDigestAsk): Promise<Float64Array[]>;

// src/render/groundShapePool.ts:49-51 method GroundShapePool.pads
pads(ask: PadsAsk): Promise<(Pad | null)[]>;

// src/render/groundShapePool.ts:62-73 method GroundShapePool.run
private run(job: GroundJob): Promise<GroundJobAnswer>;

// src/render/groundShapePool.ts:75-107 method GroundShapePool.start
private start(): Lane[] | null;
