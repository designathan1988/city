// What the feature uses of src/render/bakePool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/bakePool.ts:127-134 function run
/** A job queued for the next free worker; null at once where no worker can run it. */
function run(message: object, transfer: Transferable[] = [], people = false): Promise<BakeReply | null>;

// src/render/bakePool.ts:137-138 function clipsOf
/** The clips as baked in a worker, their head bones as matrices again. */
const clipsOf = (clips: readonly (BakedClip | null)[]): (ClipFrames | undefined)[] => …;

// src/render/bakePool.ts:198-201 function releaseIdleBakeWorkers
/** Releases decoded motion libraries held by idle workers after all visual bodies leave. */
export function releaseIdleBakeWorkers(): void;
