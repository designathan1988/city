// What the feature uses of src/world/buildings/recordTable.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/recordTable.ts:468-471 interface RecordRef
/** Where a building's record is read from: a table and its row (posted to a worker as the table's buffer and the row). */
export interface RecordRef {
  readonly buffer: ArrayBufferLike;
  readonly row: number;
}
