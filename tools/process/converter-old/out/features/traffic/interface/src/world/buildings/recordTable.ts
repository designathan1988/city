// What the feature uses of src/world/buildings/recordTable.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/recordTable.ts:302-333 class RecordTable
/** A table read back from its buffer (posted or shared): its columns are views on it; its records are decoded by a `TableReader`. */
export class RecordTable { … }

// src/world/buildings/recordTable.ts:303-303 field RecordTable.rows
readonly rows: number;

// src/world/buildings/recordTable.ts:304-304 field RecordTable.ids
readonly ids: Float64Array;

// src/world/buildings/recordTable.ts:330-332 method RecordTable.reader
/** A reader of this table: what it decodes (strings, shapes, shared members) is kept for its own life only (a job's). `weak`: its shared members kept only while a record read holds them (`TableReader`). */
  reader(weak = false): TableReader;

// src/world/buildings/recordTable.ts:336-465 class TableReader
/** DO NOT keep a reader past the job it was made for: a stateless job decodes what it reads and lets it go; the shared members it decoded are one object for all its rows (as `intern.ts` on the page) */
export class TableReader { … }

// src/world/buildings/recordTable.ts:377-381 method TableReader.record
/** A row's record: a fresh object of its own, its shared members the reader's. */
  record(row: number): Building;

// src/world/buildings/recordTable.ts:468-471 interface RecordRef
/** Where a building's record is read from: a table and its row (posted to a worker as the table's buffer and the row). */
export interface RecordRef {
  readonly buffer: ArrayBufferLike;
  readonly row: number;
}

// src/world/buildings/recordTable.ts:493-500 function refOf
/** Where a worker reads a record: its tile's row, or a table of its own (an edited or new building, or one opened by the cut). */
export function refOf(b: Building, kept = true): RecordRef;

// src/world/buildings/recordTable.ts:508-515 function eachRecord
/** Records read from their refs one at a time, as `readRecords` reads them: DO NOT hold them all where each is used once - what is worked out of a record lives as long as it does (`identityMemo`, the WeakMaps of `geometry.ts`), and a plate's mass job held its four tiles' buildings with all their layouts (~110 MB a dense tile) in a worker sharing the page's heap cage (`jobs/jobPool.ts`, 2026-10-17). */
export function* eachRecord(refs: readonly RecordRef[]): Generator<Building, void, void>;
