// What the feature uses of src/editor/preparedCity.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/editor/preparedCity.ts:71-74 interface OpenedCity
/** A city as the page opens it; `table`: its buildings' table (`prepareCity`), the document's source of truth as it is (`BuildingStore.loadSections`), null: its buildings are in `data`. */
export interface OpenedCity {
  readonly data: SerializedDoc;
  readonly table: ArrayBufferLike | null;
}

// src/editor/preparedCity.ts:82-84 function openedBuildings
/** How many buildings an opened city has, in its data or its table. */
export function openedBuildings(city: OpenedCity): number;
