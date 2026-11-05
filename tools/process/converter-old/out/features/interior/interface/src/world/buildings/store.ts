// What the feature uses of src/world/buildings/store.ts (shared with the program): declarations as the source writes them, bodies left out.

// src/world/buildings/store.ts:32-441 class BuildingStore
/** DO NOT copy a record on the way in again "to be safe": the deep copy of every record added or replaced */
export class BuildingStore { … }

// src/world/buildings/store.ts:34-34 field BuildingStore.items
/** @mapWide Every building of the map: the table the map was opened from, read when asked, and the records written over it (`BuildingItems`). A working copy's lies over the original's (`forkFrom`). */
  private items: Map<BuildingId, Building> = new BuildingItems();

// src/world/buildings/store.ts:56-56 constructor BuildingStore.constructor
/** `journal`: the document's diary; a store on its own keeps one of its own. */
  constructor(private readonly journal: ChangeJournal = new ChangeJournal());

// src/world/buildings/store.ts:64-66 getter BuildingStore.revision
/** The serial of the latest change of the buildings in the diary (their shape, place or use; not their wear). */
  get revision(): number;

// src/world/buildings/store.ts:99-101 getter BuildingStore.size
get size(): number;

// src/world/buildings/store.ts:115-117 method BuildingStore.get
get(id: BuildingId): Building | undefined;

// src/world/buildings/store.ts:119-121 method BuildingStore.has
has(id: BuildingId): boolean;

// src/world/buildings/store.ts:123-125 method BuildingStore.all
all(): IterableIterator<Building>;

// src/world/buildings/store.ts:128-130 method BuildingStore.idList
/** Every building's id, in the order of `all()`, no record decoded (the table's id column, then the ids written after it). */
  idList(): IterableIterator<BuildingId>;

// src/world/buildings/store.ts:133-137 method BuildingStore.near
/** The buildings whose box (or anchor) may reach the rectangle, in the order of `all()` */
  near(minX: number, minY: number, maxX: number, maxY: number): Building[];

// src/world/buildings/store.ts:148-150 getter BuildingStore.nextId
/** The id the next `add` will hand out. */
  get nextId(): BuildingId;

// src/world/buildings/store.ts:156-164 method BuildingStore.add
/** @perItem Stores a new building under a fresh id. */
  add(value: Omit<Building, 'id'>): Building;

// src/world/buildings/store.ts:167-176 method BuildingStore.put
/** @perItem Replaces an existing building's record (same id). */
  put(building: Building): void;

// src/world/buildings/store.ts:246-254 method BuildingStore.remove
/** @perItem Runs once a building removed: nothing it reaches may walk the map. */
  remove(id: BuildingId): boolean;
