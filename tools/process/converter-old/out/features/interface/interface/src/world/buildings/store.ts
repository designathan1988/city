// What the feature uses of src/world/buildings/store.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/store.ts:32-441 class BuildingStore
/** DO NOT copy a record on the way in again "to be safe": the deep copy of every record added or replaced */
export class BuildingStore { … }

// src/world/buildings/store.ts:34-34 field BuildingStore.items
/** @mapWide Every building of the map: the table the map was opened from, read when asked, and the records written over it (`BuildingItems`). A working copy's lies over the original's (`forkFrom`). */
  private items: Map<BuildingId, Building> = new BuildingItems();

// src/world/buildings/store.ts:99-101 getter BuildingStore.size
get size(): number;

// src/world/buildings/store.ts:115-117 method BuildingStore.get
get(id: BuildingId): Building | undefined;

// src/world/buildings/store.ts:123-125 method BuildingStore.all
all(): IterableIterator<Building>;
