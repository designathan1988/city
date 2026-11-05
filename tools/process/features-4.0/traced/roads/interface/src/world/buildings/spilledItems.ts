// What the feature uses of src/world/buildings/spilledItems.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/spilledItems.ts:16-161 class SpilledItems
/**
 * THE BUILDINGS OF A CITY BEING GROWN, THE OLDER ONES IN A TABLE (`TableWriter`), NOT AS OBJECTS (`editor/smallCityMap.ts` buildCityTable):
 * a `Map` to the store (`BuildingStore.spillToTable`), in a `Map`'s order (the table's rows in the order they were added, then the records
 * still held; a row set again keeps its place). `spill` moves all but the newest records into the table, each record read back from it
 * decoded when asked (JSON-equal to the record written: settled records, `settleBuilding`). DO NOT hold every record of a large city as
 * an object: ~20 KB of JS heap a building, 160,000 buildings took the city's worker to 5.6 GB, past the V8 cage it shares with its page
 * (2026-10-22).
 */
export class SpilledItems implements Map<BuildingId, Building> { … }
