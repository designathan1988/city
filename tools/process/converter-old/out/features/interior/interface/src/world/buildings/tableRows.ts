// What the feature uses of src/world/buildings/tableRows.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/tableRows.ts:10-122 class TableRows
/** DO NOT keep every record decoded (`tableRecords` did: ~8 KB of heap a building, the table ~1.3 KB outside it). A record read is the same object while anyone holds it (a `WeakRef` by row while it lives), so a reader that compares a held record with the store's sees what it saw when every record was an object; one nobody holds is collected and decoded again when asked. */
export class TableRows implements ReadonlyMap<BuildingId, Building> { … }

// src/world/buildings/tableRows.ts:44-46 getter TableRows.buffer
get buffer(): ArrayBufferLike;

// src/world/buildings/tableRows.ts:53-62 method TableRows.rowOf
/** The row of an id; -1 when the table has none. */
  rowOf(id: number): number;
