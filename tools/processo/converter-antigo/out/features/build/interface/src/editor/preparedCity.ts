// What the feature uses of src/editor/preparedCity.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/preparedCity.ts:10-13 interface PreparedCity
/** A WHOLE CITY MADE OFF THE PAGE (`cityGen.worker.ts`), ITS BUILDINGS ONE SHARED TABLE AS A TILE'S (`tileStore.ts` prepareTile): each migrated and settled where it is made (`settleBuilding`: made plain, its members interned), the page reading them from the table, each knowing its row */

/** A city's document without its buildings, and their table (`prepareCity`, `PreparedCityWriter`). */
export interface PreparedCity {
  readonly data: SerializedDoc;
  readonly buildings: ArrayBufferLike;
}

// src/editor/preparedCity.ts:19-19 variable PREPARE_BATCH
/** DO NOT post a city's buildings as objects: the page held the message's clone and its migrated copy, walked and interned every record again on adopting it, and made a table of its own for each building the first time a job or a write asked for it (`refOf`): ~1.3 GB allocated opening the large city, 2026-10-20. */
// DO NOT `intern` a whole map's records in one synchronous pass (`settleBuilding` interning): every member got a WeakRef, a finalizer and
// a freeze, held until the task ended - reopening a saved city of 8,102 buildings spent 0.85 s of the page in it (`npm run diagnose`,
// 2026-10-22). Written as a generated city's are (`PreparedCityWriter`, the members shared a batch at a time): the same table.
const PREPARE_BATCH = 2000;

// src/editor/preparedCity.ts:21-31 function prepareCity
export function prepareCity(data: SerializedDoc): PreparedCity;

// src/editor/preparedCity.ts:38-68 class PreparedCityWriter
/**
 * A CITY'S BUILDINGS PREPARED AS `prepareCity` PREPARES THEM, A BATCH AT A TIME AS THEY ARE MADE (`smallCityMap.ts` buildCityTable): each
 * migrated, the first of an id kept, settled and written to the page's table (`TableWriter`), the records let go. The same records as
 * `prepareCity`'s table (the members a building shares with others written once a batch, not once a city: the same values).
 */
export class PreparedCityWriter { … }

// src/editor/preparedCity.ts:39-39 field PreparedCityWriter.table
private readonly table = new TableWriter(true);

// src/editor/preparedCity.ts:46-48 method PreparedCityWriter.endBatch
/** The batch of members shared ended (`SharedMembers.end`): the table keeps as nodes only the members kept. */
  endBatch(): void;

// src/editor/preparedCity.ts:51-62 method PreparedCityWriter.add
/** The next records of the city, in its order. */
  add(batch: readonly Building[]): void;

// src/editor/preparedCity.ts:65-67 method PreparedCityWriter.finish
/** The table of every record added. */
  finish(): ArrayBufferLike;

// src/editor/preparedCity.ts:71-74 interface OpenedCity
/** A city as the page opens it; `table`: its buildings' table (`prepareCity`), the document's source of truth as it is (`BuildingStore.loadSections`), null: its buildings are in `data`. */
export interface OpenedCity {
  readonly data: SerializedDoc;
  readonly table: ArrayBufferLike | null;
}

// src/editor/preparedCity.ts:77-79 function preparedCity
/** A prepared city as the page opens it: DO NOT decode its table here (every record an object, ~8 KB of heap a building): the document reads a row when asked. */
export function preparedCity(data: SerializedDoc, table: ArrayBufferLike): OpenedCity;

// src/editor/preparedCity.ts:82-84 function openedBuildings
/** How many buildings an opened city has, in its data or its table. */
export function openedBuildings(city: OpenedCity): number;
