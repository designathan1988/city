// What the feature uses of src/world/buildings/items.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/items.ts:6-142 class BuildingItems
/** THE STORE'S BUILDINGS: A RECORD TABLE (`TableRows`, decoded when read) WITH THE RECORDS WRITTEN SINCE OVER IT. A `Map` to its readers, in a `Map`'s order (a key set again keeps its place, one deleted and set again goes last); its writes told before they happen while an edit is recorded (`before`, as `core/recordMap.ts`). */
export class BuildingItems implements Map<BuildingId, Building> { … }

// src/world/buildings/items.ts:12-12 field BuildingItems.gone
/** Ids of the table deleted (those set again are in `added` too). */
  private gone = new Set<BuildingId>();

// src/world/buildings/items.ts:16-16 constructor BuildingItems.constructor
constructor(private base: TableRows | null = null);

// src/world/buildings/items.ts:51-53 method BuildingItems.owns
/** Whether the record of `key` was written here (not the table's own). */
  owns(key: BuildingId): boolean;
