// What the feature uses of src/world/buildings/store.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/store.ts:133-137 method BuildingStore.near
/** The buildings whose box (or anchor) may reach the rectangle, in the order of `all()` */
  near(minX: number, minY: number, maxX: number, maxY: number): Building[];

// src/world/buildings/store.ts:246-254 method BuildingStore.remove
/** @perItem Runs once a building removed: nothing it reaches may walk the map. */
  remove(id: BuildingId): boolean;
