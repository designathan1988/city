// What the feature uses of src/editor/saveStore.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/saveStore.ts:6-14 interface PartStore
/** THE SAVES' STORE (IndexedDB) AND ITS BATCHES: what the save worker runs (`save.worker.ts`) and what the map by tiles and the tile store write through (`chunkedSave.ts`, `tileStore.ts`) - apart from the worker, so neither imports the other */

/** Where a map's blobs are kept: blobs by key, written and removed in one go. */
export interface PartStore {
  get(keys: readonly string[]): Promise<(Blob | null)[]>;
  /** Writes `put` and removes `remove` together, with `marks` (short texts, `marks`); false when nothing was written. */
  write(put: readonly (readonly [string, Blob])[], remove: readonly string[], marks?: readonly (readonly [string, string])[]): Promise<boolean>;
  /** Every mark whose key starts with `prefix`, in one read: what a world's every tile is known by (`tileBlob.ts` tileMarkKey); absent: the store keeps none, each tile is asked. */
  marks?(prefix: string): Promise<readonly (readonly [string, string])[]>;
  /** Who encodes and writes a save for this store (a worker); absent: done where it is asked. */
  readonly writer?: SaveWriter;
}

// src/editor/saveStore.ts:17-17 interface SaveEntry
/** One blob of a save: its key and its binary frame (`saveBinary.ts`), compressed by the writer when `packed`. A plain ArrayBuffer, transferred to the writer, never copied (MDN "Transferable objects"). */
export interface SaveEntry { readonly key: string; readonly bytes: ArrayBuffer; readonly packed: boolean }

// src/editor/saveStore.ts:19-19 interface SaveBatch
/** What one save writes and removes, in one transaction: the frames of what changed, never the map. */
export interface SaveBatch { readonly put: readonly SaveEntry[]; readonly remove: readonly string[] }

// src/editor/saveStore.ts:23-23 interface SaveWriter
/** Writes a save's batch, true once it is stored. */
export interface SaveWriter { write(batch: SaveBatch): Promise<boolean> }
