// What the feature uses of src/world/buildings/store.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/store.ts:32-441 class BuildingStore
/** DO NOT copy a record on the way in again "to be safe": the deep copy of every record added or replaced */
export class BuildingStore { … }

// src/world/buildings/store.ts:115-117 method BuildingStore.get
get(id: BuildingId): Building | undefined;

// src/world/buildings/store.ts:123-125 method BuildingStore.all
all(): IterableIterator<Building>;

// src/world/buildings/store.ts:128-130 method BuildingStore.idList
/** Every building's id, in the order of `all()`, no record decoded (the table's id column, then the ids written after it). */
  idList(): IterableIterator<BuildingId>;

// src/world/buildings/store.ts:133-137 method BuildingStore.near
/** The buildings whose box (or anchor) may reach the rectangle, in the order of `all()` */
  near(minX: number, minY: number, maxX: number, maxY: number): Building[];

// src/world/buildings/store.ts:140-145 method BuildingStore.nearAny
/** The buildings `meets` keeps for one of `rects`, asked of those within `pad` of it, each once, in the store's order */
  nearAny(rects: readonly (readonly [number, number, number, number])[], pad: number,
    meets: (b: Building, rect: readonly [number, number, number, number]) => boolean): Building[];

// src/world/buildings/store.ts:303-305 method BuildingStore.sections
/** The buildings as buffers (`BuildingSections`): the table shared, the records written encoded - the save's, the simulation's, a whole map's undo. */
  sections(): BuildingSections;
