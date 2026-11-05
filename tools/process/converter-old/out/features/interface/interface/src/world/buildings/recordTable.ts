// What the feature uses of src/world/buildings/recordTable.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/recordTable.ts:17-20 function allocate
/** Shared memory where the page is cross-origin isolated (and in workers and tests); a plain buffer otherwise (only read on its own thread). */
/** DO NOT trust the constructor alone: a context not isolated may still define it, and posting its buffer throws (MDN "crossOriginIsolated"; Node's tests have no such flag). */
export function allocate(bytes: number): ArrayBufferLike;

// src/world/buildings/recordTable.ts:23-27 function tableIn
/** A table's bytes in a buffer of their own (shared where the thread is isolated, `allocate`): one read back from a store, which keeps plain buffers only (IndexedDB refuses shared memory, HTML "StructuredSerialize"). */
export function tableIn(bytes: ArrayBufferLike): ArrayBufferLike;

// src/world/buildings/recordTable.ts:108-112 function encodeRecords
/** The rows written as one table: their values as JSON would keep them (undefined members left out, in arrays null), numbers exact. */
export function encodeRecords(rows: readonly Building[]): RecordTable;

// src/world/buildings/recordTable.ts:302-333 class RecordTable
/** A table read back from its buffer (posted or shared): its columns are views on it; its records are decoded by a `TableReader`. */
export class RecordTable { … }

// src/world/buildings/recordTable.ts:303-303 field RecordTable.rows
readonly rows: number;

// src/world/buildings/recordTable.ts:312-321 constructor RecordTable.constructor
constructor(readonly buffer: ArrayBufferLike);

// src/world/buildings/recordTable.ts:468-471 interface RecordRef
/** Where a building's record is read from: a table and its row (posted to a worker as the table's buffer and the row). */
export interface RecordRef {
  readonly buffer: ArrayBufferLike;
  readonly row: number;
}

// src/world/buildings/recordTable.ts:484-490 function tableRecords
/** DO NOT read a whole city's table this way on the page (`world/buildings/tableRows.ts` reads a row when asked): every record decoded and kept is ~8 KB of heap a building. A tile's buildings read from its table, each knowing its row. */
export function tableRecords(buffer: ArrayBufferLike): Building[];
