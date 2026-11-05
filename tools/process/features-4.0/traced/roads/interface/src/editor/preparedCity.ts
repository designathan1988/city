// What the feature uses of src/editor/preparedCity.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/preparedCity.ts:10-13 interface PreparedCity
/** A WHOLE CITY MADE OFF THE PAGE (`cityGen.worker.ts`), ITS BUILDINGS ONE SHARED TABLE AS A TILE'S (`tileStore.ts` prepareTile): each migrated and settled where it is made (`settleBuilding`: made plain, its members interned), the page reading them from the table, each knowing its row */

/** A city's document without its buildings, and their table (`prepareCity`, `PreparedCityWriter`). */
export interface PreparedCity {
  readonly data: SerializedDoc;
  readonly buildings: ArrayBufferLike;
}
