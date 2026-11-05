// What the feature uses of src/render/bakePool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/bakePool.ts:20-20 function usable
/** Whether the page gave the bake service's lanes (`pagePorts.ts`): none, every bake runs where it is asked. */
const usable = (): boolean => …;

// src/render/bakePool.ts:21-21 variable releaseRequested
let releaseRequested = false;

// src/render/bakePool.ts:37-43 function releaseIfIdle
function releaseIfIdle(): void;

// src/render/bakePool.ts:127-134 function run
/** A job queued for the next free worker; null at once where no worker can run it. */
function run(message: object, transfer: Transferable[] = [], people = false): Promise<BakeReply | null>;
