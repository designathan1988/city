// What the feature uses of src/world/buildings/recordTable.ts (shared with the program): declarations as the source writes them, bodies left out.

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
