// What the feature uses of src/world/buildings/store.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/store.ts:32-441 class BuildingStore
/** DO NOT copy a record on the way in again "to be safe": the deep copy of every record added or replaced */
export class BuildingStore { … }

// src/world/buildings/store.ts:99-101 getter BuildingStore.size
get size(): number;

// src/world/buildings/store.ts:115-117 method BuildingStore.get
get(id: BuildingId): Building | undefined;

// src/world/buildings/store.ts:119-121 method BuildingStore.has
has(id: BuildingId): boolean;

// src/world/buildings/store.ts:133-137 method BuildingStore.near
/** The buildings whose box (or anchor) may reach the rectangle, in the order of `all()` */
  near(minX: number, minY: number, maxX: number, maxY: number): Building[];

// src/world/buildings/store.ts:179-188 method BuildingStore.wear
/** @perItem DO NOT write a wear change with `put` */
  wear(id: BuildingId, decay: number): void;

// src/world/buildings/store.ts:246-254 method BuildingStore.remove
/** @perItem Runs once a building removed: nothing it reaches may walk the map. */
  remove(id: BuildingId): boolean;
