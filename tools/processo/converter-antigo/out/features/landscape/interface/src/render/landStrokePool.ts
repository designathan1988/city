// What the feature uses of src/render/landStrokePool.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/render/landStrokePool.ts:10-25 function askLandStroke
/** `job` run by the pool's first free worker, its buffers handed over (the asker sends copies); `failed`: the asker does it on the page. */
export function askLandStroke<J extends LandStrokeJob>(job: J, transfer: Transferable[], done: (answer: LandStrokeAnswer<J>) => void,
  failed: (message: string) => void): JobTicket;
