// What the feature uses of src/editor/save.worker.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/save.worker.ts:9-9 type SaveMessage
type SaveMessage = { readonly id: number; readonly batch: SaveBatch };

// src/editor/save.worker.ts:12-23 function serve
/** This service of the job system (`job.worker.ts`): each batch encoded and written, answered by its id. */
export function serve(self: JobScope): void;
