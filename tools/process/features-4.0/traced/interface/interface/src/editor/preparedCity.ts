// What the feature uses of src/editor/preparedCity.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/preparedCity.ts:10-13 interface PreparedCity
/** A WHOLE CITY MADE OFF THE PAGE (`cityGen.worker.ts`), ITS BUILDINGS ONE SHARED TABLE AS A TILE'S (`tileStore.ts` prepareTile): each migrated and settled where it is made (`settleBuilding`: made plain, its members interned), the page reading them from the table, each knowing its row */

/** A city's document without its buildings, and their table (`prepareCity`, `PreparedCityWriter`). */
export interface PreparedCity {
  readonly data: SerializedDoc;
  readonly buildings: ArrayBufferLike;
}

// src/editor/preparedCity.ts:21-31 function prepareCity
export function prepareCity(data: SerializedDoc): PreparedCity;

// src/editor/preparedCity.ts:71-74 interface OpenedCity
/** A city as the page opens it; `table`: its buildings' table (`prepareCity`), the document's source of truth as it is (`BuildingStore.loadSections`), null: its buildings are in `data`. */
export interface OpenedCity {
  readonly data: SerializedDoc;
  readonly table: ArrayBufferLike | null;
}

// src/editor/preparedCity.ts:77-79 function preparedCity
/** A prepared city as the page opens it: DO NOT decode its table here (every record an object, ~8 KB of heap a building): the document reads a row when asked. */
export function preparedCity(data: SerializedDoc, table: ArrayBufferLike): OpenedCity;
